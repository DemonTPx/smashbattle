#include <SDL/SDL.h>

#include "Gameplay.h"
#include "LaserBeam.h"

#include "LaserBeamPowerUp.h"
#include "commands/CommandGeneratePowerup.h"

#include "Main.h"

LaserBeamPowerUp::LaserBeamPowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position, Main &main) : GameplayObject(main), main_(main) {
	clip->x = 112;
	clip->y = 0;
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
	main_.audio->play(SND_ITEM, p->position->x);

	shoot_laserbeam(p, main_);

	done = true;
}

void LaserBeamPowerUp::shoot_laserbeam(Player * p, Main &main) {
	LaserBeam * lb;

	std::vector<int> targets;
	std::vector<int> target_hp;
	int num_targets;
	int i, tmp;
	bool swapped;

	targets.reserve(main.gameplay().players->size());
	target_hp.reserve(main.gameplay().players->size());
	
	// Get all target players
	num_targets = 0;
	for(i = 0; i < (int)main.gameplay().players->size(); i++) {
		if(main.gameplay().players->at(i) == p)
			continue;
		if(main.gameplay().players->at(i)->is_dead)
			continue;
		targets[num_targets] = i;
		target_hp[num_targets] = main.gameplay().players->at(i)->hitpoints;
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
		lb = new LaserBeam(main);
		lb->owner = p;
		lb->target = main.gameplay().players->at(targets[i]);
		lb->start += (30 * i);

		main.gameplay().add_object(lb);
	}
}

void LaserBeamPowerUp::hit_npc(NPC * npc) {}

void LaserBeamPowerUp::draw_impl(SDL_Surface * screen, int frames_processed) {
	SDL_BlitSurface(surface, clip, screen, position);
}

void LaserBeamPowerUp::move(Level * level) {
	if(!level->is_on_bottom(position)) {
		done = true;
	}
}

void LaserBeamPowerUp::process() {}

void LaserBeamPowerUp::copyTo(network::CommandGeneratePowerup &powerup)
{
	GameplayObject::copyTo(powerup);

	powerup.data.type = network::CommandGeneratePowerup::PowerUps::TypeLaserBeam;
}