#include "PowerUp.h"
#include "BombPowerUp.h"

BombPowerUp::BombPowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position, int ammount) {
	this->surface = surface;
	this->clip = clip;
	this->position = position;
	this->ammount = ammount;
}

void BombPowerUp::cleanup() {
	delete clip;
	delete position;
}

void BombPowerUp::got_powerup(Player * p) {
	p->bombs += ammount;
	if(p->bombs > 9)
		p->bombs = 9;
}

void BombPowerUp::show(SDL_Surface * screen) {
	SDL_BlitSurface(surface, clip, screen, position);
}
