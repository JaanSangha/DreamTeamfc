#include "Engine.h"

int main(int argc, char* args[]) // Main MUST have these parameters for SDL.
{
	return Engine::Instance().Run(); // invokes run of engine 
}