#include <SDL/SDL.h>

#include "Gameplay.h"
#include "Airstrike.h"

#include "AirstrikePowerUp.h"

AirstrikePowerUp::AirstrikePowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position) {
	this->surface = surface;
	this->clip = clip;
	this->position = position;
	is_powerup = true;
}

AirstrikePowerUp::~AirstrikePowerUp() {
	delete clip;
	delete position;
}

void AirstrikePowerUp::hit_player(Player * p) {
	Airstrike * as;

	Main::audio->play(SND_ITEM, p->position->x);

	as = new Airstrike();
	as->owner = p;

	Gameplay::instance->add_object(as);

	done = true;
}

void AirstrikePowerUp::hit_npc(NPC * npc) {}

void AirstrikePowerUp::draw(SDL_Surface * screen) {
	SDL_BlitSurface(surface, clip, screen, position);
}

void AirstrikePowerUp::move(Level * level) {
	if(!level->is_on_bottom(position)) {
		done = true;
	}
}

void AirstrikePowerUp::process() {}
