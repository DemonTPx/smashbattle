#ifndef _LASERBEAMPOWERUP_H
#define _LASERBEAMPOWERUP_H

#include "GameplayObject.h"

class LaserBeamPowerUp : public GameplayObject {
public:
	LaserBeamPowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position);
	~LaserBeamPowerUp();

	virtual void move(Level * level);
	virtual void process();

	virtual void hit_player(Player * player);
	virtual void hit_npc(NPC * npc);

	virtual void draw(SDL_Surface * screen);
	
	SDL_Surface * surface;
	SDL_Rect * clip;
};

#endif
