#include "Engine.h"
#include "Utilities.h"
#include <iostream>
#include <ctime>
#define WIDTH 1024
#define HEIGHT 768
#define FPS 60
#define BGSCROLL 4
#define PSPEED 6
using namespace std;

Engine::Engine() :m_bRunning(false), m_bSpaceOk(true) { std::cout << "Engine class constructed!" << endl; }
Engine::~Engine() {}

bool Engine::Init(const char* title, int xpos, int ypos, int width, int height, int flags)
{
	std::cout << "Initializing game." << endl;
	srand((unsigned)time(NULL));
	// Attempt to initialize SDL.
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		// Create the window.
		m_pWindow = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
		if (m_pWindow != nullptr) // Window init success.
		{
			m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, 0);
			if (m_pRenderer != nullptr) // Renderer init success.
			{
				if (IMG_Init(IMG_INIT_PNG))
				{
					m_pPlayerText = IMG_LoadTexture(m_pRenderer, "IMG/Player.png");
					m_pBGText = IMG_LoadTexture(m_pRenderer, "Img/Backgrounds.png");
					m_pObsText = IMG_LoadTexture(m_pRenderer, "IMG/Obstacles.png");
				
				//m_pSprText = IMG_LoadTexture(m_pRenderer, "Img/sprites.png");
			
				if (Mix_Init(MIX_INIT_MP3) != 0) // Mixer init success.
				{
					Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 2048); // Good for most games.
					Mix_AllocateChannels(16);
					m_pMusic = Mix_LoadMUS("Aud/game.mp3"); // Load the music track.
					// Load the chunks into the Mix_Chunk vector.
					m_vSounds.reserve(3); // Optional but good practice.
					m_vSounds.push_back(Mix_LoadWAV("Aud/enemy.wav"));
					m_vSounds.push_back(Mix_LoadWAV("Aud/laser.wav"));
					m_vSounds.push_back(Mix_LoadWAV("Aud/explode.wav"));
					/* By the way, you should check to see if any of these loads are failing and
					   you can use Mix_GetError() to print out the error message. Wavs can be finicky.*/
				}
				else return false;
				}
				else return false; //Image init fail
			}
			else return false; // Renderer init fail.
		}
		else return false; // Window init fail.
	}
	else return false; // SDL init fail.
	m_fps = (Uint32)round((1 / (double)FPS) * 1000); // Sets FPS in milliseconds and rounds.
	m_iKeystates = SDL_GetKeyboardState(nullptr);
	m_pFSM = new FSM();
	m_pFSM->ChangeState(new TitleState());
	srand((unsigned)time(NULL)); // Seed random number sequence.
	m_vec.reserve(8);
	// Create the vector now.
	for (int i = 0; i < 9; i++)
	{
		Object* temp = new Object({ 128,128,128,128 }, { 128 * i, 384,128,128 }); // 9empty boxes
		m_vec.push_back(temp);
	}
	/*bgArray[0] = { {0,0,1024,768}, {0, 0, 1024, 768} };
	bgArray[1] = { {0,0,1024,768}, {1024, 0, 1024, 768} };*/
	/*mgArray[0] = { {0,0,256,768}, {0, 0, 256, 768} };
	mgArray[1] = { {0,0,256,768}, {256, 0, 256, 768} };*/
	m_pPlayer = new Player({ 0,256,128,128 }, { 1024 / 2 - 64, 300, 128, 128 });
	m_pPlatforms[0] = new Platform({ -300,520,2024,100 });
	//m_pObstacles[0] = new Object({ 0,0,125,445 }, { obDst.x, 0, 125, 445 });
	//background 
	bgDst = {0,0,1024, 768};
	bgDstTwo = { bgDst.x+1024,0,1024, 768 };
	bgSrc = {0,0,1024, 768 };
	mgDst = { 0,0,256, 768 };
	mgDstTwo = { mgDst.x + 256,0,256, 768 };
	mgDstThree = { mgDstTwo.x + 256,0,256, 768 };
	mgDstFour = { mgDstThree.x + 256,0,256, 768 };
	mgDstFive = { mgDstFour.x + 256,0,256, 768 };
	mgDstSix = { mgDstFive.x + 256,0,256, 768 };
	mgSrc = { 1024,0,256, 768 };
	fgDst = { 0,0,256, 768 };
	fgDstTwo = { fgDst.x + 256,0,256, 768 };
	fgDstThree = { fgDstTwo.x + 256,0,256, 768 };
	fgDstFour = { fgDstThree.x + 256,0,256, 768 };
	fgDstFive = { fgDstFour.x + 256,0,256, 768 };
	fgDstSix = { fgDstFive.x + 256,0,256, 768 };
	fgSrc = { 1280,0,256, 768 };
	//Mix_PlayMusic(m_pMusic, -1); // Play. -1 = looping.
	Mix_VolumeMusic(16); // 0-MIX_MAX_VOLUME (128).
	m_bRunning = true; // Everything is okay, start the engine.
	//bool enemyInt = false;
	std::cout << "Success!" << endl;
	return true;
}

void Engine::Wake()
{
	m_start = SDL_GetTicks();
}

void Engine::Sleep()
{
	m_end = SDL_GetTicks();
	m_delta = m_end - m_start;
	if (m_delta < m_fps) // Engine has to sleep.
		SDL_Delay(m_fps - m_delta);
}

void Engine::HandleEvents()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT: // User pressed window's 'x' button.
			m_bRunning = false;
			break;
		case SDL_KEYDOWN: // Try SDL_KEYUP instead.
			if (event.key.keysym.sym == SDLK_ESCAPE)
				m_bRunning = false;
			break;
		case SDL_KEYUP:
			if (event.key.keysym.sym == SDLK_SPACE)
				m_bSpaceOk = true;
			if (event.key.keysym.sym == SDLK_a || event.key.keysym.sym == SDLK_d)
				m_pPlayer->SetAccelX(0.0);
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button >= 1 && event.button.button <3)
				m_MouseState[event.button.button -1] = true;
			break;
		case SDL_MOUSEBUTTONUP:
			if (event.button.button >= 1 && event.button.button < 3)
				m_MouseState[event.button.button - 1] = false;
			break;
		case SDL_MOUSEMOTION:
			SDL_GetMouseState(&m_MousePos.x, &m_MousePos.y);
			break;
		}
		
	}
}

// Keyboard utility function. 
bool Engine::KeyDown(SDL_Scancode c)
{
	if (m_iKeystates != nullptr)
	{
		if (m_iKeystates[c] == 1)
			return true;
		else
			return false;
	}
	return false;
}


void Engine::CheckCollision()
{
	for (int i = 0; i < 1; i++)
	{	//this checks the player is on the ground
		if (SDL_HasIntersection(m_pPlayer->GetDstP(), m_pPlatforms[i]->GetDstP()))
		{
			if ((m_pPlayer->GetDstP()->y + m_pPlayer->GetDstP()->h) - m_pPlayer->GetVelY() <= m_pPlatforms[i]->GetDstP()->y)
			{ // Collision from top.
				m_pPlayer->SetGrounded(true);
				if (m_pPlayer->GetAnimState() == jumping)
					m_pPlayer->SetRunning();
				m_pPlayer->SetVelY(0.0); // Stop the player from moving vertically. We aren't modifying gravity.
				m_pPlayer->SetY(m_pPlatforms[i]->GetDstP()->y - m_pPlayer->GetDstP()->h - 1);
			}

			break;
		}
	/*	if (Object.x + Object.w)
		{
		}*/
		//if (SDL_HasIntersection(m_pPlayer->GetDstP(), m_pObstacles[i]->GetDstP()))
		//{
		//	if (m_pPlayer->GetDstP()->y - m_pPlayer->GetVelY() >= m_pObstacles[i]->GetDstP()->y + m_pObstacles[i]->GetDstP()->h)
		//	{ // Collision from bottom.
		//		m_pPlayer->SetVelY(0.0); // Stop the player from moving vertically. We aren't modifying gravity.
		//		//m_pPlayer->SetY(m_pPlatforms[i]->GetDstP()->y + m_pPlatforms[i]->GetDstP()->h + 1);
		//	}
		//	else if ((m_pPlayer->GetDstP()->x + m_pPlayer->GetDstP()->w) - m_pPlayer->GetVelX() <= m_pObstacles[i]->GetDstP()->x)
		//	{ // Collision from left.
		//		m_pPlayer->SetVelX(0.0); // Stop the player from moving horizontally.
		//		//m_pPlayer->SetX(m_pPlatforms[i]->GetDstP()->x - m_pPlayer->GetDstP()->w - 1);
		//	}
		//	else if (m_pPlayer->GetDstP()->x - m_pPlayer->GetVelX() >= m_pObstacles[i]->GetDstP()->x + m_pObstacles[i]->GetDstP()->w)
		//	{ // Collision from right.
		//		m_pPlayer->SetVelX(0.0); // Stop the player from moving horizontally.
		//		//m_pPlayer->SetX(m_pPlatforms[i]->GetDstP()->x + m_pPlatforms[i]->GetDstP()->w + 1);
		//	}
		//	break;
		//}
	
	}
	//// Player vs. Enemy.
	//SDL_Rect p = { m_player->GetDstP()->x-100, m_player->GetDstP()->y, 100, 94 };
	//for (int i = 0; i < (int)m_vEnemies.size(); i++)
	//{
	//	SDL_Rect e = { m_vEnemies[i]->GetDstP()->x, m_vEnemies[i]->GetDstP()->y-40, 56, 40 };
	//	if (SDL_HasIntersection(&p, &e))
	//	{
	//		// Game over!
	//		cout << "Player goes boom!" << endl;
	//		Mix_PlayChannel(-1, m_vSounds[2], 0);
	//		GetFSML().ChangeState(new LoseState());
	//		break;
	//	}
	//}
}

/* Update is SUPER way too long on purpose! Part of the Assignment 1, if you use
   this program as a start project is to chop up Update and figure out where each
   part of the code is supposed to go. A practice in OOP is to have objects handle
   their own behaviour and this is a big hint for you. */
void Engine::Update()
{
	randNum = rand() % 3;
	m_pFSM->Update();

	if (dynamic_cast<GameState*>(Engine::Instance().GetFSM().GetStates().back()))
	{

		//// Scroll the backgrounds. Check if they need to snap back.
		if (bgDst.x != -1024)
		{
			bgDst.x = bgDst.x - BGSCROLL;
			bgDstTwo.x = bgDstTwo.x - BGSCROLL;
		}
		else if (bgDst.x == -1024)
		{
			bgDst.x = 0;
			bgDstTwo.x = 1024;
		}

		if (mgDst.x != -256)
		{
			mgDst.x = mgDst.x - BGSCROLL;
			mgDstTwo.x = mgDstTwo.x - BGSCROLL;
			mgDstThree.x = mgDstThree.x - BGSCROLL;
			mgDstFour.x = mgDstFour.x - BGSCROLL;
			mgDstFive.x = mgDstFive.x - BGSCROLL;
			mgDstSix.x = mgDstSix.x - BGSCROLL;
		}
		else if (mgDst.x == -256)
		{
			mgDst.x = 0;
			mgDstTwo.x = 256;
			mgDstThree.x = mgDstTwo.x +256;
			mgDstFour.x = mgDstThree.x + 256;
			mgDstFive.x = mgDstFour.x + 256;
			mgDstSix.x = mgDstFive.x + 256;
		}
		if (fgDst.x != -256)
		{
			fgDst.x = mgDst.x - BGSCROLL;
			fgDstTwo.x = mgDstTwo.x - BGSCROLL;
			fgDstThree.x = mgDstThree.x - BGSCROLL;
			fgDstFour.x = mgDstFour.x - BGSCROLL;
			fgDstFive.x = mgDstFive.x - BGSCROLL;
			fgDstSix.x = mgDstSix.x - BGSCROLL;
		}
		else if (fgDst.x == -256)
		{
			fgDst.x = 0;
			fgDstTwo.x = 256;
			fgDstThree.x = mgDstTwo.x + 256;
			fgDstFour.x = mgDstThree.x + 256;
			fgDstFive.x = mgDstFour.x + 256;
			fgDstSix.x = mgDstFive.x + 256;
		}
		// Check for out of bounds.
		if ((m_vec[0])->GetX() <= -128) // Fully off-screen.
		{
			// Clean the first element in the vector.
			delete m_vec[0];
			m_vec[0] = nullptr;
			m_vec.erase(m_vec.begin()); // Pop the front element.

		// Create a new box at the end.
			Object* temp;
			// you are going to have to implement random obstacles
			if (m_spawnCtr++ == 0)
			{
				//randNum = rand() % 3;
				//random choice of obstacle in here.
				//get random number
				if (randNum == 0)
				{
					//floor spikes
					temp = new Object({ 128,62,128,64 }, { 1024, 450,128,62 }, true); //not empty
				}
				else if (randNum == 1)
				{
					//circular saw
					temp = new Object({ 128,128,128,128 }, { 1024, 448,128,128 }, true); //not empty
				}
				else
				{
					//spike wall..
					temp = new Object({ 0,0,125,445 }, { 1024, 0,128,445 }, true); //not empty
				}
			}
			else
				temp = new Object({ 128,128,128,128 }, { 1024, 384,128,128 }); // empty

			m_vec.push_back(temp);
			if (m_spawnCtr == 3)
				m_spawnCtr = 0;
		}
		// Scroll the objects.
		for (int col = 0; col < 9; col++)
		{
			m_vec[col]->Update();
		}

		//for (int i = 0; i < 2; i++)
		//{
		//	bgDst.x - BGSCROLL;
		//	mgArray[i].GetDstP()->x -= BGSCROLL;
		//}
		//if (bgArray[1].GetDstP()->x <= 0)
		//{
		//	bgArray[0].GetDstP()->x = 0;
		//	bgArray[1].GetDstP()->x = 1024;
		//}
		// Player animation/movement.
		switch (m_pPlayer->GetAnimState())
		{
		case running:
			if (KeyDown(SDL_SCANCODE_S))
				m_pPlayer->SetRolling();
			else if (KeyDown(SDL_SCANCODE_SPACE) && m_bSpaceOk /*&& m_pPlayer->IsGrounded()*/)
			{
				m_bSpaceOk = false; // This just prevents repeated jumps when holding spacebar.
				m_pPlayer->SetAccelY(-JUMPFORCE); // Sets the jump force.
				m_pPlayer->SetGrounded(false);
				m_pPlayer->SetJumping();
			}
			break;
		case rolling:
			if (!KeyDown(SDL_SCANCODE_S))
				m_pPlayer->SetRunning();
			break;
		}

		if (KeyDown(SDL_SCANCODE_A))
		{
			if (m_pPlayer->GetDstP()->x > 10)
			{
				m_pPlayer->SetDir(-1);
				m_pPlayer->MoveX();
			}
		}
		else if (KeyDown(SDL_SCANCODE_D))
		{
			if (m_pPlayer->GetDstP()->x <1010)
			{
				m_pPlayer->SetDir(1);
				m_pPlayer->MoveX();
			}
		}

		m_pPlayer->Update();
		m_pPlayer->SetAccelY(0.0); // After jump, reset vertical acceleration.

		CheckCollision();

	}
}

/* In the render I commented out some lines that rendered the original destination rectangles
   for the sprites so I can show you the after-effect of rotating via SDL_RenderCopyEX(). In 
   order to do collision, I manually created new rectangles for collision in the CheckCollision method. */
void Engine::Render()
{
	m_pFSM->Render();
	if (dynamic_cast<TitleState*>(Engine::Instance().GetFSM().GetStates().back())) {

	}
	else if (dynamic_cast<GameState*>(Engine::Instance().GetFSM().GetStates().back()))
	{
		SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 255);
		SDL_RenderClear(m_pRenderer); // Clear the screen with the draw color.
		// Render stuff. Background first.
		//for (int i = 0; i < 2; i++)
		//{
			SDL_RenderCopy(m_pRenderer, m_pBGText, &bgSrc, &bgDst );
			SDL_RenderCopy(m_pRenderer, m_pBGText, &bgSrc, &bgDstTwo);
			SDL_RenderCopy(m_pRenderer, m_pBGText, &mgSrc, &mgDst);
			SDL_RenderCopy(m_pRenderer, m_pBGText, &mgSrc, &mgDstTwo);
			SDL_RenderCopy(m_pRenderer, m_pBGText, &mgSrc, &mgDstThree);
			SDL_RenderCopy(m_pRenderer, m_pBGText, &mgSrc, &mgDstFour);
			SDL_RenderCopy(m_pRenderer, m_pBGText, &mgSrc, &mgDstFive);
			SDL_RenderCopy(m_pRenderer, m_pBGText, &mgSrc, &mgDstSix);
			// Render obstacles.
			for (int col = 0; col < 9; col++)
			{
				m_vec[col]->Render();
			}
			SDL_RenderCopy(m_pRenderer, m_pBGText, &fgSrc, &fgDst);
			SDL_RenderCopy(m_pRenderer, m_pBGText, &fgSrc, &fgDstTwo);
			SDL_RenderCopy(m_pRenderer, m_pBGText, &fgSrc, &fgDstThree);
			SDL_RenderCopy(m_pRenderer, m_pBGText, &fgSrc, &fgDstFour);
			SDL_RenderCopy(m_pRenderer, m_pBGText, &fgSrc, &fgDstFive);
			//obstacle
			SDL_RenderCopy(m_pRenderer, m_pOBText, &obSrc, &obDst);
		
			/*SDL_RenderCopy(m_pRenderer, m_pBGText, mgArray[i].GetSrcP(), mgArray[i].GetDstP());*/
		//}
			
		// render the player sprite
		SDL_RenderCopy(m_pRenderer, m_pPlayerText, m_pPlayer->GetSrcP(), m_pPlayer->GetDstP());
	
		SDL_RenderPresent(m_pRenderer);
	}
}

void Engine::Clean()
{
	std::cout << "Cleaning game." << endl;
	m_pFSM->Clean();
	delete m_pFSM;
	m_pFSM = nullptr; //optional
	//delete m_player;
	delete m_pPlayer;
	//m_player = nullptr;
	for (int col = 0; col < 9; col++)
	{
		delete m_vec[col]; // vector.erase() won't deallocate memory through pointer.
		m_vec[col] = nullptr; // Optional again, but good practice.
	}
	SDL_DestroyTexture(m_pPlayerText);
	SDL_DestroyRenderer(m_pRenderer);
	SDL_DestroyWindow(m_pWindow);
	for (int i = 0; i < (int)m_vSounds.size(); i++)
		Mix_FreeChunk(m_vSounds[i]);
	m_vSounds.clear();
	Mix_FreeMusic(m_pMusic);
	Mix_CloseAudio();
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}

int Engine::Run()
{
	if (Init("GAME1017 Shooter Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0) == false)
		return 1;
	while (m_bRunning)
	{
		Wake();
		HandleEvents();
		Update();
		Render();
		if (m_bRunning)
			Sleep();
	}
	Clean();
	return 0;
}

Engine& Engine::Instance()
{
	static Engine instance; //object of the engine class
	return instance;
}

SDL_Renderer* Engine::GetRenderer() {return m_pRenderer;}
SDL_Texture* Engine::GetTexture(){return m_pObsText;}

FSM& Engine::GetFSM() { return *m_pFSM; }

SDL_Point& Engine::GetMousePos() { return m_MousePos; }

bool Engine::GetMouseState(int idx) { return m_MouseState[idx]; }
void Engine::QuitGame() { m_bRunning = false; }
