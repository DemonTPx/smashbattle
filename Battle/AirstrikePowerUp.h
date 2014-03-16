#pragma once

#include "GameplayObject.h"

namespace network {
class CommandGeneratePowerup;
}

class AirstrikePowerUp : public GameplayObject {
public:
	AirstrikePowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position, Main &main);
	~AirstrikePowerUp();

	virtual void move(Level * level);
	virtual void process();

	virtual void hit_player(Player * player);
	virtual void hit_npc(NPC * npc);

	virtual void draw(SDL_Surface * screen, int frames_processed = 0);

	virtual void copyTo(network::CommandGeneratePowerup &powerup);

	static void shoot_airstrike(Player * p, Main &main);
	

	SDL_Surface * surface;
	SDL_Rect * clip;

	Main &main_;
};
