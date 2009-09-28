#ifndef _AIRSTRIKEPOWERUP_H
#define _AIRSTRIKEPOWERUP_H

#include "GameplayObject.h"

class AirstrikePowerUp : public GameplayObject {
public:
	AirstrikePowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position);
	~AirstrikePowerUp();

	virtual void move(Level * level);
	virtual void process();

	virtual void hit_player(Player * player);
	virtual void hit_npc(NPC * npc);

	virtual void draw(SDL_Surface * screen);
	
	SDL_Surface * surface;
	SDL_Rect * clip;
};

#endif
