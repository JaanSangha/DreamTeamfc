#include "FSM.h"
#include "Engine.h"
#include <iostream>
#include <vector>
#include <functional>

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
GameState::GameState(){}

void GameState::Enter()
{
	cout << "Entering Game.." << endl;
}

void GameState::Update()
{
	if (Engine::Instance().KeyDown(SDL_SCANCODE_P))
		Engine::Instance().GetFSM().PushState(new PauseState());
	else if (Engine::Instance().KeyDown(SDL_SCANCODE_X))
		Engine::Instance().GetFSM().ChangeState(new TitleState());
	//else if (Engine::Instance()
}

void GameState::Render()
{
	/*cout << "Rendering Game.." << endl;
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 0, 255, 0, 255);
	SDL_RenderClear(Engine::Instance().GetRenderer());
	if (dynamic_cast<GameState*>(Engine::Instance().GetFSM().GetStates().back()))
		State::Render();*/
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
	m_pTitleBackground = IMG_LoadTexture(Engine::Instance().GetRenderer(), "Img/TitleScreen.png");
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

