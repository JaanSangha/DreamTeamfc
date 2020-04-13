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
			if (event.key.keysym.sym == SDLK_l)
				Engine::Instance().GetFSM().ChangeState(new LoseState);
			break;
		case SDL_KEYUP:
			if (event.key.keysym.sym == SDLK_SPACE)
				m_bSpaceOk = true;
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
	
}


/* Update is SUPER way too long on purpose! Part of the Assignment 1, if you use
   this program as a start project is to chop up Update and figure out where each
   part of the code is supposed to go. A practice in OOP is to have objects handle
   their own behaviour and this is a big hint for you. */
void Engine::Update()
{
	m_pFSM->Update();
	
}

/* In the render I commented out some lines that rendered the original destination rectangles
   for the sprites so I can show you the after-effect of rotating via SDL_RenderCopyEX(). In 
   order to do collision, I manually created new rectangles for collision in the CheckCollision method. */
void Engine::Render()
{
	m_pFSM->Render();
}

void Engine::Clean()
{
	std::cout << "Cleaning game." << endl;
	m_pFSM->Clean();
	delete m_pFSM;
	m_pFSM = nullptr; //optional
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

bool Engine::spaceChecker()
{
	if (m_bSpaceOk == true)
	{
		return true;
	}
	else
	{
		return false;
	}
}

SDL_Renderer* Engine::GetRenderer() {return m_pRenderer;}
SDL_Texture* Engine::GetTexture(){return m_pObsText;}

FSM& Engine::GetFSM() { return *m_pFSM; }

SDL_Point& Engine::GetMousePos() { return m_MousePos; }

bool Engine::GetMouseState(int idx) { return m_MouseState[idx]; }
void Engine::QuitGame() { m_bRunning = false; }
