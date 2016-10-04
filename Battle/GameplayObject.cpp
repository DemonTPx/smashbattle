#include "SDL2/SDL.h"

#include "network/Server.h"
#include "Level.h"
#include "Player.h"
#include "GameplayObject.h"
#include "commands/CommandGeneratePowerup.h"
#include "commands/CommandRemovePowerup.hpp"
#include "Main.h"

GameplayObject::GameplayObject(Main &main) : Drawable(main), main_(main) {
	done = false;
	is_powerup = false;
	id_ = 0;
}

GameplayObject::~GameplayObject() 
{
	if (is_powerup && main_.getServer().active()) {
		network::CommandRemovePowerup remo;
		remo.data.time = main_.getServer().getServerTime();
		remo.data.powerup_id = id();
		main_.getServer().sendAll(remo);
	}
}

void GameplayObject::copyTo(network::CommandGeneratePowerup &powerup)
{
	memcpy(&powerup.data.position, this->position, sizeof(SDL_Rect));
	powerup.data.powerupid = id();
}