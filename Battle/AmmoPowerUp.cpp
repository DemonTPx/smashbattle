#include "PowerUp.h"
#include "AmmoPowerUp.h"

AmmoPowerUp::AmmoPowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position, int ammo) {
	this->surface = surface;
	this->clip = clip;
	this->position = position;
	this->ammo = ammo;
}

void AmmoPowerUp::cleanup() {
	delete clip;
	delete position;
}

void AmmoPowerUp::got_powerup(Player * p) {
	p->bullets += ammo;
	if(p->bullets > 50)
		p->bullets = 50;
}

void AmmoPowerUp::show(SDL_Surface * screen) {
	SDL_BlitSurface(surface, clip, screen, position);
}
