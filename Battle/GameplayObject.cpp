#include "SDL/SDL.h"

#include "Level.h"
#include "Player.h"
#include "GameplayObject.h"
#include "commands/CommandGeneratePowerup.h"

GameplayObject::GameplayObject() {
	done = false;
	is_powerup = false;
}

GameplayObject::~GameplayObject() {
}

void GameplayObject::copyTo(CommandGeneratePowerup &powerup)
{
	memcpy(&powerup.data.position, this->position, sizeof(SDL_Rect));
}