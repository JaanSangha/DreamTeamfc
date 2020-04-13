#include "FSM.h"
#include "Engine.h"
#include <iostream>
#include <vector>
#include <functional>
#include <ctime>
#define BGSCROLL 2
#define MGSCROLL 4
#define PSPEED 6

using namespace std;

//Begin state
void State::Render()
{
	SDL_RenderPresent(Engine::Instance().GetRenderer());
}

void State::Resume(){}
// end state

//pause state
PauseState::PauseState()
{
}

void PauseState::Enter()
{
	cout << "Entering Pause.." << endl;
	m_vButtons.push_back(new ResumeButton("Img/ResumeButtons.png", { 0,0,196,79 }, { 412,200,200,80 }));
	// This exit button has a different size but SAME function as the one in title.
	m_vButtons.push_back(new ExitButton("Img/ExitButtons.png", { 0,0,196,79 }, { 412,400,200,80 }));
}

void PauseState::Update()
{
	for (int i = 0; i < (int)m_vButtons.size(); i++)
		m_vButtons[i]->Update();
}

void PauseState::Render()
{
	cout << "Rendering Pause.." << endl;
	Engine::Instance().GetFSM().GetStates().front()->Render();
	SDL_SetRenderDrawBlendMode(Engine::Instance().GetRenderer(), SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 128, 128, 128, 128);
	Engine::Instance().GetFSM().GetStates().front()->Render();
	SDL_Rect rect = { 336, 128, 350, 450 };
	SDL_RenderFillRect(Engine::Instance().GetRenderer(), &rect);
	
	for (int i = 0; i < (int)m_vButtons.size(); i++)
		m_vButtons[i]->Render();
	State::Render();
}


void PauseState::Exit()
{
	cout << "Exiting Pause.." << endl;
	for (int i = 0; i < (int)m_vButtons.size(); i++)
	{
		delete m_vButtons[i];
		m_vButtons[i] = nullptr;
	}
	m_vButtons.clear();
	m_vButtons.shrink_to_fit();
}
//end pausestate

//begin gamestate
GameState::GameState()
{
	m_pPlayerText = IMG_LoadTexture(Engine::Instance().GetRenderer(), "IMG/Player.png");
	m_pBGText = IMG_LoadTexture(Engine::Instance().GetRenderer(), "Img/Backgrounds.png");
	m_pObsText = IMG_LoadTexture(Engine::Instance().GetRenderer(), "IMG/Obstacles.png");
	Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 2048); // Good for most games.
	Mix_AllocateChannels(16);
	m_vSounds.reserve(2); // Optional but good practice.
	m_vSounds.push_back(Mix_LoadWAV("Aud/jump.wav"));
	m_vSounds.push_back(Mix_LoadWAV("Aud/death.wav"));

	srand((unsigned)time(NULL)); // Seed random number sequence.
	m_vec.reserve(8);
	// Create the vector now.
	for (int i = 0; i < 9; i++)
	{
		Object* temp = new Object({ 128,128,128,128 }, { 128 * i, 384,128,128 }); // 9empty boxes
		m_vec.push_back(temp);
	}
	m_pPlayer = new Player({ 0,256,128,128 }, { 1024 / 2 - 64, 300, 128, 128 });
	m_pPlatforms[0] = new Platform({ -300,520,2024,100 });
	ColBox = { m_pPlayer->GetDstP()->x,m_pPlayer->GetDstP()->y,m_pPlayer->GetDstP()->w,m_pPlayer->GetDstP()->h };

	//background 
	bgDst = { 0,0,1024, 768 };
	bgDstTwo = { bgDst.x + 1024,0,1024, 768 };
	bgSrc = { 0,0,1024, 768 };
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
}

void GameState::Enter()
{
	cout << "Entering Game.." << endl;
}

void GameState::CheckCollision()
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
	}

	for (int col = 0; col < 9; col++)
	{

		SDL_Rect* temp = m_vec[col]->GetDstP();

			if (m_vec[col]->getSprite() == true && SDL_HasIntersection(&ColBox, temp))
			{
				//Engine::Instance().GetFSM().PushState(new LoseState());
				
					// Collision from left.
					if ((ColBox.x+ ColBox.w) - m_pPlayer->GetVelX() <= temp->x)
					{ 
						Mix_PlayChannel(-1, m_vSounds[1], 0);
						m_pPlayer->SetVelX(0.0); // Stop the player from moving horizontally.
						m_pPlayer->SetAccelX(0.0);
						m_pPlayer->SetX(temp->x - ColBox.w - 1);
						cout << "colliding from left" << endl;
						Engine::Instance().GetFSM().PushState(new LoseState());
						//	m_pPlayer->SetDeath();
					}
					// Collision from right.
					else if (ColBox.x - m_pPlayer->GetVelX() >= temp->x +temp->w)
					{ 
						Mix_PlayChannel(-1, m_vSounds[1], 0);
						m_pPlayer->SetVelX(0.0); // Stop the player from moving horizontally.
						m_pPlayer->SetAccelX(0.0);
						m_pPlayer->SetX(temp->x + temp->w + 1);
						cout << "colliding from right" << endl;
						Engine::Instance().GetFSM().PushState(new LoseState());
						//m_pPlayer->SetDeath();
					
					}
					// Collision from bottom.
					else if (ColBox.y - m_pPlayer->GetVelY() >= temp->y + temp->h)
					{ 
						Mix_PlayChannel(-1, m_vSounds[1], 0);
						m_pPlayer->SetVelY(0.0); // Stop the player from moving vertically. We aren't modifying gravity.
						m_pPlayer->SetAccelY(0.0);
						cout << "colliding from bottom" << endl;
						Engine::Instance().GetFSM().PushState(new LoseState());
						//m_pPlayer->SetDeath();
						
					}
					// Collision from top.
					else if ((ColBox.y + ColBox.h) - m_pPlayer->GetVelY() <= temp->y)
					{ 
						Mix_PlayChannel(-1, m_vSounds[1], 0);
						m_pPlayer->SetGrounded(true);
						if (m_pPlayer->GetAnimState() == jumping)
							m_pPlayer->SetRunning();
						m_pPlayer->SetVelY(0.0); // Stop the player from moving vertically. We aren't modifying gravity.
						m_pPlayer->SetY(temp->y - ColBox.h - 1);
						cout << "colliding from top" << endl;
						Engine::Instance().GetFSM().PushState(new LoseState());

					}				
			}
	}
}

void GameState::Update()
{

	randNum = rand() % 3;

	if (dynamic_cast<GameState*>(Engine::Instance().GetFSM().GetStates().back()))
	{
		ColBox = { m_pPlayer->GetDstP()->x,m_pPlayer->GetDstP()->y,110,128 };
		cout << ColBox.h << endl;
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
			mgDst.x = mgDst.x - MGSCROLL;
			mgDstTwo.x = mgDstTwo.x - MGSCROLL;
			mgDstThree.x = mgDstThree.x - MGSCROLL;
			mgDstFour.x = mgDstFour.x - MGSCROLL;
			mgDstFive.x = mgDstFive.x - MGSCROLL;
			mgDstSix.x = mgDstSix.x - MGSCROLL;
		}
		else if (mgDst.x == -256)
		{
			mgDst.x = 0;
			mgDstTwo.x = 256;
			mgDstThree.x = mgDstTwo.x + 256;
			mgDstFour.x = mgDstThree.x + 256;
			mgDstFive.x = mgDstFour.x + 256;
			mgDstSix.x = mgDstFive.x + 256;
		}
		if (fgDst.x != -256)
		{
			fgDst.x = mgDst.x - MGSCROLL;
			fgDstTwo.x = mgDstTwo.x - MGSCROLL;
			fgDstThree.x = mgDstThree.x - MGSCROLL;
			fgDstFour.x = mgDstFour.x - MGSCROLL;
			fgDstFive.x = mgDstFive.x - MGSCROLL;
			fgDstSix.x = mgDstSix.x - MGSCROLL;
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
					temp = new Object({ 128,62,128,64 }, { 1024, 456,100,62 }, true); //not empty
				}
				else if (randNum == 1)
				{
					//circular saw
					temp = new Object({ 128,128,128,128 }, { 1024, 448,100,128 }, true); //not empty
				}
				else if (randNum == 2)
				{
					//spike wall..
					temp = new Object({ 0,0,125,445 }, { 1024, 0,128,450 }, true); //not empty
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

		// Player animation/movement.
		switch (m_pPlayer->GetAnimState())
		{
		case running:
			if (Engine::Instance().KeyDown(SDL_SCANCODE_S))
			{
				m_pPlayer->SetRolling();
			}
			else if (Engine::Instance().KeyDown(SDL_SCANCODE_SPACE) && Engine::Instance().spaceChecker() == true  /*&& m_pPlayer->IsGrounded()*/)
			{
				m_bSpaceOk = false; // This just prevents repeated jumps when holding spacebar.
				m_pPlayer->SetAccelY(-JUMPFORCE); // Sets the jump force.
				m_pPlayer->SetGrounded(false);
				m_pPlayer->SetJumping();
				Mix_PlayChannel(-1, m_vSounds[0], 0);
			}
			break;
		case rolling:
			if (Engine::Instance().KeyDown(SDL_SCANCODE_S))
			{
				ColBox.h = 64;
			}
			else if (!Engine::Instance().KeyDown(SDL_SCANCODE_S))
			{
				m_pPlayer->SetRunning();
				ColBox.h = m_pPlayer->GetDstP()->h;
			}
			break;
		}

		if (Engine::Instance().KeyDown(SDL_SCANCODE_A))
		{
			if (m_pPlayer->GetDstP()->x > 10)
			{
				m_pPlayer->SetDir(-1);
				m_pPlayer->MoveX();
			}
		}
		else if (Engine::Instance().KeyDown(SDL_SCANCODE_D))
		{
			if (m_pPlayer->GetDstP()->x < 1010)
			{
				m_pPlayer->SetDir(1);
				m_pPlayer->MoveX();
			}
		}
		else if (!Engine::Instance().KeyDown(SDL_SCANCODE_A))
		{
			m_pPlayer->SetAccelX(0.0);
		}
		else if (!Engine::Instance().KeyDown(SDL_SCANCODE_D))
		{
			m_pPlayer->SetAccelX(0.0);
		}

		m_pPlayer->Update();
		m_pPlayer->SetAccelY(0.0); // After jump, reset vertical acceleration.

		CheckCollision();

	}

	if (Engine::Instance().KeyDown(SDL_SCANCODE_P))
		Engine::Instance().GetFSM().PushState(new PauseState());
	else if (Engine::Instance().KeyDown(SDL_SCANCODE_X))
		Engine::Instance().GetFSM().ChangeState(new TitleState());
	//else if (Engine::Instance()
}

void GameState::Render()
{

	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 0, 0, 0, 255);
	SDL_RenderClear(Engine::Instance().GetRenderer()); // Clear the screen with the draw color.
/*	SDL_RenderDrawRect(Engine::Instance().GetRenderer(), &ColBox);*/
	// Render stuff. Background first.
	//for (int i = 0; i < 2; i++)
	//{
	SDL_RenderCopy(Engine::Instance().GetRenderer(), m_pBGText, &bgSrc, &bgDst);
	SDL_RenderCopy(Engine::Instance().GetRenderer(), m_pBGText, &bgSrc, &bgDstTwo);
	SDL_RenderCopy(Engine::Instance().GetRenderer(), m_pBGText, &mgSrc, &mgDst);
	SDL_RenderCopy(Engine::Instance().GetRenderer(), m_pBGText, &mgSrc, &mgDstTwo);
	SDL_RenderCopy(Engine::Instance().GetRenderer(), m_pBGText, &mgSrc, &mgDstThree);
	SDL_RenderCopy(Engine::Instance().GetRenderer(), m_pBGText, &mgSrc, &mgDstFour);
	SDL_RenderCopy(Engine::Instance().GetRenderer(), m_pBGText, &mgSrc, &mgDstFive);
	SDL_RenderCopy(Engine::Instance().GetRenderer(), m_pBGText, &mgSrc, &mgDstSix);
	// Render obstacles.
	for (int col = 0; col < 9; col++)
	{
		m_vec[col]->Render();
	}
	SDL_RenderCopy(Engine::Instance().GetRenderer(), m_pBGText, &fgSrc, &fgDst);
	SDL_RenderCopy(Engine::Instance().GetRenderer(), m_pBGText, &fgSrc, &fgDstTwo);
	SDL_RenderCopy(Engine::Instance().GetRenderer(), m_pBGText, &fgSrc, &fgDstThree);
	SDL_RenderCopy(Engine::Instance().GetRenderer(), m_pBGText, &fgSrc, &fgDstFour);
	SDL_RenderCopy(Engine::Instance().GetRenderer(), m_pBGText, &fgSrc, &fgDstFive);
	//obstacle
	SDL_RenderCopy(Engine::Instance().GetRenderer(), m_pOBText, &obSrc, &obDst);

	/*SDL_RenderCopy(m_pRenderer, m_pBGText, mgArray[i].GetSrcP(), mgArray[i].GetDstP());*/
//}

// render the player sprite
	SDL_RenderCopy(Engine::Instance().GetRenderer(), m_pPlayerText, m_pPlayer->GetSrcP(), m_pPlayer->GetDstP());

	SDL_RenderPresent(Engine::Instance().GetRenderer());

}

void GameState::Exit()
{
	cout << "Exiting Game.." << endl;
}

void GameState::Resume()
{
	cout << "Resuming Game.." << endl;
}
// end game state

//begin titlestate
TitleState::TitleState(){}

void TitleState::Enter()
{
	cout << "Entering Title.." << endl;
	m_vButtons.push_back(new PlayButton("Img/StartButtons.png", { 0, 0, 196, 79 }, { 382,415,250,100 }));
		//std::bind( &FSM::ChangeState, &Engine::Instance().GetFSM(), new GameState() )));
	// For the bind: what function, what instance, any parameters.
	m_vButtons.push_back(new ExitButton("Img/ExitButtons.png", { 0,0,196,79 }, { 382,520,250,100 }));
}

void TitleState::Update()
{
	for (int i = 0; i < (int)m_vButtons.size(); i++)
		m_vButtons[i]->Update();
}

void TitleState::Render()
{
	cout << "Rendering Title.." << endl;
	tSrc = tDst = { 0,0,1024,768 };
	m_pTitleBackground = IMG_LoadTexture(Engine::Instance().GetRenderer(), "Img/DesertTitle.png");
	SDL_RenderCopy(Engine::Instance().GetRenderer(), m_pTitleBackground, &tSrc, &tDst); //make titlescreen appear
	for (int i = 0; i < (int)m_vButtons.size(); i++)
		m_vButtons[i]->Render();
	State::Render();

	/*SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(),255,0,0,255);
	SDL_RenderClear(Engine::Instance().GetRenderer());*/
}

void TitleState::Exit()
{
	cout << "Exiting Title.." << endl;
	for (int i = 0; i < (int)m_vButtons.size(); i++)
	{
		delete m_vButtons[i];
		m_vButtons[i] = nullptr;
	}
	m_vButtons.clear();
	m_vButtons.shrink_to_fit();
}
//end titlestate

//begin losestate
LoseState::LoseState(){}

void LoseState::Enter()
{
	cout << "Entering Lose.." << endl;
	m_vButtons.push_back(new MenuButton("Img/MenuButtons.png", { 0, 0, 196, 79 }, { 382,485,250,100 }));
	m_vButtons.push_back(new ExitButton("Img/ExitButtons.png", { 0,0,196,79 }, { 382,610,250,100 }));
}

void LoseState::Update()
{
	for (int i = 0; i < (int)m_vButtons.size(); i++)
		m_vButtons[i]->Update();
}

void LoseState::Render()
{
	cout << "Rendering Lose.." << endl;
	lSrc = lDst = { 0,0,1024,768 };
	m_pLoseBackground = IMG_LoadTexture(Engine::Instance().GetRenderer(), "Img/GameOver1.png");
	SDL_RenderCopy(Engine::Instance().GetRenderer(), m_pLoseBackground, &lSrc, &lDst); //make losescreen appear
	for (int i = 0; i < (int)m_vButtons.size(); i++)
		m_vButtons[i]->Render();
	State::Render();
}

void LoseState::Exit()
{
	cout << "Exiting lose.." << endl;
	for (int i = 0; i < (int)m_vButtons.size(); i++)
	{
		delete m_vButtons[i];
		m_vButtons[i] = nullptr;
	}
	m_vButtons.clear();
	m_vButtons.shrink_to_fit();
}
//endloseState


//begin FSM

void FSM::Update()
{
	if (!m_vStates.empty())
		m_vStates.back()->Update(); //invokes the update method of the current state
}
void FSM::Render()
{
	if (!m_vStates.empty())
		m_vStates.back()->Render(); //invokes the render method of the current state
}
void FSM::ChangeState(State* pState)
{
	if (!m_vStates.empty())
	{
		m_vStates.back()->Exit(); // invokes exit of current state
		delete m_vStates.back(); //deallocates current state
		m_vStates.back() = nullptr;
		m_vStates.pop_back();
	}
	PushState(pState);
	//pState->Enter(); // invoke enter of the new current state
	//m_vStates.push_back(pState); // push the adress of the new current state into the vector
}
void FSM::PushState(State* pState)
{
	pState->Enter(); // invoke enter of the new current state
	m_vStates.push_back(pState);
	m_vStates.back()->Enter();
}
void FSM::PopState() // e.g. pause state to game state
{
	if (!m_vStates.empty())
	{
		m_vStates.back()->Exit(); // invokes exit of current state
		delete m_vStates.back(); //deallocates current state
		m_vStates.back() = nullptr;
		m_vStates.pop_back();
	}
	m_vStates.back()->Resume();
}
void FSM::Clean()
{
	while (!m_vStates.empty())
	{
		m_vStates.back()->Exit(); // invokes exit of current state
		delete m_vStates.back(); //deallocates current state
		m_vStates.back() = nullptr;
		m_vStates.pop_back();
	}
}
vector<State*>& FSM::GetStates() { return m_vStates; }
//end FSM

