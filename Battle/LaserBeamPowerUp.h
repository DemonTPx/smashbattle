#pragma once

#include "GameplayObject.h"
namespace network {
class CommandGeneratePowerup;
}

class LaserBeamPowerUp : public GameplayObject {
public:
	LaserBeamPowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position, Main &main);
	~LaserBeamPowerUp();

	virtual void move(Level * level);
	virtual void process();

	virtual void hit_player(Player * player);
	virtual void hit_npc(NPC * npc);

	virtual void copyTo(network::CommandGeneratePowerup &powerup);

	static void shoot_laserbeam(Player * player, Main &main);
	
	SDL_Surface * surface;
	SDL_Rect * clip;

	Main &main_;

protected:

	virtual void draw_impl(SDL_Surface * screen, int frames_processed = 0);

};
