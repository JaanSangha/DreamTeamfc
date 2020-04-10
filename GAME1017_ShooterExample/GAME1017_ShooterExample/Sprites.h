#pragma once
#include "SDL.h"
#include <iostream>
using namespace std;

enum state { running, jumping, rolling, dying };

class Sprite
{
protected:
	SDL_Rect m_rSrc, m_rDst;

public:
	Sprite(const SDL_Rect s, const SDL_Rect d);
	SDL_Rect* GetSrcP();
	SDL_Rect* GetDstP();
};

class Player : public Sprite
{
private:
	bool m_bGrounded;
	double m_dAccelX,
		m_dMaxAccelX,
		m_dAccelY,
		m_dVelX,
		m_dMaxVelX,
		m_dVelY,
		m_dMaxVelY,
		m_dDrag,
		m_dGrav,
		m_dXForce;
	state m_state; // which animation state character is in.
	int m_iDir, // Direction. -1 or 1. Not being used in this example.
		m_iSprite, // which sprite to display 
		m_iSpriteMin, // min starting sprite index in image
		m_iSpriteMax, //  max sprite position in image
		m_iFrame, //frame counter
		m_iFrameMax, // number of frames to display each sprite
		m_iAnimY;  //the starting y value for animations. 256
	void SetAnimationState(state st, int y, int fmax, int smin, int smax);
public:
	Player(const SDL_Rect s, const SDL_Rect d);
	void Update();
	void Animate();
	void MoveX();
	void Stop();
	void SetDir(int dir);
	void SetAccelX(double a);
	void SetAccelY(double a);
	bool IsGrounded();
	void SetGrounded(bool g);
	double GetVelX();
	double GetVelY();
	void SetVelX(double v);
	void SetVelY(double v);
	void SetX(int y);
	void SetY(int y);
	const state GetAnimState();
	void SetJumping();
	void SetRunning();
	void SetRolling();
	void SetDeath();
};

// Platform doesn't have much in it right now. 
class Platform : public Sprite
{
public:
	Platform(const SDL_Rect d) :Sprite({ 0,0,0,0 }, d) {}
};