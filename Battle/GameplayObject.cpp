#include "SDL/SDL.h"

#include "Server.h"
#include "Level.h"
#include "Player.h"
#include "GameplayObject.h"
#include "commands/CommandGeneratePowerup.h"
#include "commands/CommandRemovePowerup.hpp"

GameplayObject::GameplayObject() {
	done = false;
	is_powerup = false;
	id_ = 0;
}

GameplayObject::~GameplayObject() 
{
	if (is_powerup && Server::getInstance().active()) {
		CommandRemovePowerup remo;
		remo.data.time = Server::getInstance().getServerTime();
		remo.data.powerup_id = id();
		Server::getInstance().sendAll(remo);
	}
}

void GameplayObject::copyTo(CommandGeneratePowerup &powerup)
{
	memcpy(&powerup.data.position, this->position, sizeof(SDL_Rect));
	powerup.data.powerupid = id();
}