#include "SDL/SDL.h"

#include "GameplayObject.h"
#include "HealthPowerUp.h"

HealthPowerUp::HealthPowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position, int hp) {
	this->surface = surface;
	this->clip = clip;
	this->position = position;
	this->hp = hp;
	is_powerup = true;
}

HealthPowerUp::~HealthPowerUp() {
	delete clip;
	delete position;
}

void HealthPowerUp::hit_player(Player * p) {
	Main::audio->play(SND_ITEM, p->position->x);

	p->hitpoints += hp;

	if(p->hitpoints > 100)
		p->hitpoints = 100;

	done = true;
}

void HealthPowerUp::hit_npc(NPC * npc) {}

void HealthPowerUp::draw(SDL_Surface * screen) {
	SDL_BlitSurface(surface, clip, screen, position);
}

void HealthPowerUp::move(Level * level) {
	if(!level->is_on_bottom(position)) {
		done = true;
	}
}

void HealthPowerUp::process() {}
