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

	int targets[3];
	int target_hp[3];
	int num_targets;
	int i, tmp;
	bool swapped;

	// Get all target players
	num_targets = 0;
	for(i = 0; i < (int)Gameplay::instance->players->size(); i++) {
		if(Gameplay::instance->players->at(i) == p)
			continue;
		if(Gameplay::instance->players->at(i)->is_dead)
			continue;
		targets[num_targets] = i;
		target_hp[num_targets] = Gameplay::instance->players->at(i)->hitpoints;
		num_targets++;
	}

	// Sort: highest HP first
	do {
		swapped = false;
		for(i = 0; i < num_targets - 1; i++) {
			if(target_hp[i] < target_hp[i + 1]) {
				tmp = targets[i];
				targets[i] = targets[i + 1];
				targets[i + 1] = tmp;

				tmp = target_hp[i];
				target_hp[i] = target_hp[i + 1];
				target_hp[i + 1] = tmp;

				swapped = true;
			}
		}
	} while(swapped);

	// Create the laserbeams
	for(i = 0; i < num_targets; i++) {
		lb = new LaserBeam();
		lb->owner = p;
		lb->target = Gameplay::instance->players->at(targets[i]);
		lb->start += (30 * i);

		Gameplay::instance->add_object(lb);
	}

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
