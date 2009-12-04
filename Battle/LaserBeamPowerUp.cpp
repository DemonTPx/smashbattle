#include <SDL/SDL.h>

#include "Gameplay.h"
#include "LaserBeam.h"

#include "LaserBeamPowerUp.h"

LaserBeamPowerUp::LaserBeamPowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position) {
	this->surface = surface;
	this->clip = clip;
	this->position = position;
	is_powerup = true;
}

LaserBeamPowerUp::~LaserBeamPowerUp() {
	delete clip;
	delete position;
}

void LaserBeamPowerUp::hit_player(Player * p) {
	LaserBeam * lb;

	Main::audio->play(SND_ITEM, p->position->x);

	lb = new LaserBeam();
	lb->owner = p;
	lb->position->x = p->position->x + (PLAYER_W / 2);

	Gameplay::instance->add_object(lb);

	done = true;
}

void LaserBeamPowerUp::hit_npc(NPC * npc) {}

void LaserBeamPowerUp::draw(SDL_Surface * screen) {
	SDL_BlitSurface(surface, clip, screen, position);
}

void LaserBeamPowerUp::move(Level * level) {
	if(!level->is_on_bottom(position)) {
		done = true;
	}
}

void LaserBeamPowerUp::process() {}
