#include "SDL/SDL.h"

#include "Gameplay.h"
#include "GameplayObject.h"
#include "ShieldPowerUp.h"

ShieldPowerUp::ShieldPowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position, int time) {
	this->surface = surface;
	this->clip = clip;
	this->position = position;
	this->time = time;
	is_powerup = true;
}

ShieldPowerUp::~ShieldPowerUp() {
	delete clip;
	delete position;
}

void ShieldPowerUp::hit_player(Player * p) {
	Main::audio->play(SND_ITEM, p->position->x);

	p->is_shielded = true;
	p->shield_start = Gameplay::frame;
	p->shield_time = time;

	done = true;
}

void ShieldPowerUp::hit_npc(NPC * npc) {}

void ShieldPowerUp::draw(SDL_Surface * screen) {
	SDL_BlitSurface(surface, clip, screen, position);
}

void ShieldPowerUp::move(Level * level) {
	if(!level->is_on_bottom(position)) {
		done = true;
	}
}

void ShieldPowerUp::process() {}
