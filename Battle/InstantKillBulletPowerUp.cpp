#include "PowerUp.h"
#include "InstantKillBulletPowerUp.h"

InstantKillBulletPowerUp::InstantKillBulletPowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position, int ammo) {
	this->surface = surface;
	this->clip = clip;
	this->position = position;
	this->ammo = ammo;
}

void InstantKillBulletPowerUp::cleanup() {
	delete clip;
	delete position;
}

void InstantKillBulletPowerUp::got_powerup(Player * p) {
	p->instantkillbullets += ammo;
	if(p->instantkillbullets > 9)
		p->instantkillbullets = 9;
}

void InstantKillBulletPowerUp::show(SDL_Surface * screen) {
	SDL_BlitSurface(surface, clip, screen, position);
}
