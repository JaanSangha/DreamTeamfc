#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <vector>
#include "Sprites.h"
#include "FSM.h" 
#include "Button.h"
#define GRAV 20.0
#define JUMPFORCE 50.0
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
private:
	SDL_Texture* m_pBGText; // For the bg.
	SDL_Texture* m_pMGText; // for the mid ground
	SDL_Texture* m_pOBText; // for obstacle textures

	//obj specific properties
	SDL_Texture* m_pObsText;
	vector< Object*> m_vec; // What is this?
	int m_spawnCtr;

	// background rectangles
	SDL_Rect bgDst, bgDstTwo, mgDst, mgDstTwo, mgDstThree, mgDstFour, mgDstFive,
		mgDstSix, fgDst, fgDstTwo, fgDstThree, fgDstFour, fgDstFive, fgDstSix,
		bgSrc, mgSrc, fgSrc;

	//new from animation
	SDL_Texture* m_pPlayerText;
	bool m_bSpaceOk;
	Player* m_pPlayer;
	Platform* m_pPlatforms[1];
	Object* m_pObstacles[1];

	//obstacle test
	SDL_Rect obDst, obSrc;
	SDL_Rect ColBox;
	int randNum;

	//sounds
	vector<Mix_Chunk*> m_vSounds;
public:
	GameState();
	void Enter();
	void CheckCollision();
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

