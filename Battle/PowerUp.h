#ifndef _POWERUP_H
#define _POWERUP_H

#include "SDL/SDL.h"
#include "Player.h"

class PowerUp {
public:
	virtual void got_powerup(Player *) = 0;
	virtual void show(SDL_Surface *) = 0;

	SDL_Rect * position;
};

#endif
