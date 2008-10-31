#include "PowerUp.h"
#include "HealthPowerUp.h"

HealthPowerUp::HealthPowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position, int hp) {
	this->surface = surface;
	this->clip = clip;
	this->position = position;
	this->hp = hp;
}

HealthPowerUp::~HealthPowerUp() {
	delete clip;
	delete position;
}

void HealthPowerUp::got_powerup(Player * p) {
	p->hitpoints += hp;
	if(p->hitpoints > 100)
		p->hitpoints = 100;
}

void HealthPowerUp::show(SDL_Surface * screen) {
	SDL_BlitSurface(surface, clip, screen, position);
}
