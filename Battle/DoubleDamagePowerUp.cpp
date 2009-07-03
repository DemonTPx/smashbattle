#include "PowerUp.h"
#include "DoubleDamagePowerUp.h"

DoubleDamagePowerUp::DoubleDamagePowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position, int ammo) {
	this->surface = surface;
	this->clip = clip;
	this->position = position;
	this->ammo = ammo;
}

void DoubleDamagePowerUp::cleanup() {
	delete clip;
	delete position;
}

void DoubleDamagePowerUp::got_powerup(Player * p) {
	p->doubledamagebullets += ammo;
	if(p->doubledamagebullets > 9)
		p->doubledamagebullets = 9;
}

void DoubleDamagePowerUp::show(SDL_Surface * screen) {
	SDL_BlitSurface(surface, clip, screen, position);
}
