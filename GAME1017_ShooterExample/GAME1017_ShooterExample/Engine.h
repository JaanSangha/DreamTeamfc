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

class Engine
{
// I am avoiding in-class initialization.
private: // Private properties.
	bool m_bRunning, // Loop control flag.
		m_bENull, // These three flags check if we need to clear the respective vectors of nullptrs.
		m_bPBNull,
		m_bEBNull,
		m_bCanShoot; // This restricts the player from firing again unless they release the Spacebar.
	const Uint8* m_iKeystates; // Keyboard state container.
	Uint32 m_start, m_end, m_delta, m_fps; // Fixed timestep variables.
	SDL_Window* m_pWindow; // This represents the SDL window.
	SDL_Renderer* m_pRenderer; // This represents the buffer to draw to.

	SDL_Texture* m_pBGText; // For the bg.
	SDL_Texture* m_pMGText; // for the mid ground
	SDL_Texture* m_pOBText; // for obstacle textures

	//obj specific properties
	
	SDL_Texture* m_pObsText;
	vector< Object*> m_vec; // What is this?
	int m_spawnCtr;


	//SDL_Texture* m_pSprText; // For the sprites.
	// background rectangles
	SDL_Rect bgDst,bgDstTwo,mgDst,mgDstTwo, mgDstThree, mgDstFour, mgDstFive, mgDstSix, fgDst, fgDstTwo, fgDstThree, fgDstFour, fgDstFive, fgDstSix, bgSrc, mgSrc, fgSrc;
	//new from animation
	SDL_Texture* m_pPlayerText;
	bool m_bSpaceOk;
	Player* m_pPlayer;
	Platform* m_pPlatforms[1];
	Object* m_pObstacles[1];
	int objX = 1024;
	//obstacle test
	SDL_Rect obDst, obSrc;
	SDL_Rect ColBox;
	int randNum;
	int rollHeight;

	//SDL_Texture* m_pTitleBackground; //for titlescreen
	SDL_Point m_MousePos;
	bool m_MouseState[3] = { 0,0,0 }; //button up/down. left,middle, right

	int m_iESpawn, // The enemy spawn frame timer properties.
		m_iESpawnMax;
	//vector<Enemy*> m_vEnemies;
	SDL_Point m_pivot;

	Mix_Music* m_pMusic;
	vector<Mix_Chunk*> m_vSounds;

	FSM* m_pFSM; //pointer to a state machine instance.

private: // Private methods.
	//Engine(); //prevent object instantiation outside class
	bool Init(const char* title, int xpos, int ypos, int width, int height, int flags);
	void Wake();
	void Sleep();
	void HandleEvents();
	//void CheckCollision();
	void Update();
	void Render();
	void Clean();

public: // Public methods.
	Engine(); //prevent object instantiation outside class
	~Engine();
	int Run();
	static Engine& Instance();
	bool KeyDown(SDL_Scancode c);
	SDL_Renderer* GetRenderer();
	SDL_Texture* GetTexture();
	void CheckCollision();
	FSM& GetFSM();
	SDL_Point& GetMousePos();
	bool GetMouseState(int idx);
	void QuitGame();
	FSM& GetFSML() { return *m_pFSM; } //returns de-referenced FSM object pointer
};