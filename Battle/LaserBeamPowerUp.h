#pragma once

#include "GameplayObject.h"
class CommandGeneratePowerup;

class LaserBeamPowerUp : public GameplayObject {
public:
	LaserBeamPowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position);
	~LaserBeamPowerUp();

	virtual void move(Level * level);
	virtual void process();

	virtual void hit_player(Player * player);
	virtual void hit_npc(NPC * npc);

	virtual void draw(SDL_Surface * screen, int frames_processed = 0);

	virtual void copyTo(CommandGeneratePowerup &powerup);

	static void shoot_laserbeam(Player * player);
	
	SDL_Surface * surface;
	SDL_Rect * clip;
};
