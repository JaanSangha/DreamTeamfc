#pragma once
#include "Button.h"
#include <vector>
using namespace std;

class State // Abstract base class
{
protected:
	State() {}
public:
	virtual void Enter() = 0;
	virtual void Update() = 0;
	virtual void Render();
	virtual void Exit() = 0;
	virtual void Resume();
};

class PauseState : public State
{
private:
	SDL_Rect pSrc, pDst;
	vector<Button*> m_vButtons;
	SDL_Texture* m_pPauseBackground;
public:
	PauseState();
	void Enter();
	void Update();
	void Render();
	void Exit();
};

class GameState : public State
{ 
public:
	GameState();
	void Enter();
	void Update();
	void Render();
	void Exit();
	void Resume();
};

class TitleState : public State
{
private:
	SDL_Rect tSrc, tDst;
	vector<Button*> m_vButtons;
	SDL_Texture* m_pTitleBackground;
public:
	TitleState();
	void Enter();
	void Update();
	void Render();
	void Exit();
};

class LoseState : public State
{
private:
	SDL_Rect lSrc, lDst;
	vector<Button*> m_vButtons;
	SDL_Texture* m_pLoseBackground;
public:
	LoseState();
	void Enter();
	void Update();
	void Render();
	void Exit();
};

class FSM
{
private:
	vector<State*> m_vStates;
public:
	void Update();
	void Render();
	void ChangeState(State* pState); //normal state change
	void PushState(State* pState); //gamestate to pause state
	void PopState(); //pause state to game state
	void Clean();
	vector<State*>& GetStates();
};

