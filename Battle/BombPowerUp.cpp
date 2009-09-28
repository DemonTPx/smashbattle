#include "SDL/SDL.h"

#include "GameplayObject.h"
#include "BombPowerUp.h"

BombPowerUp::BombPowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position, int ammount) {
	this->surface = surface;
	this->clip = clip;
	this->position = position;
	this->ammount = ammount;
	is_powerup = true;
}

BombPowerUp::~BombPowerUp() {
	delete clip;
	delete position;
}

void BombPowerUp::hit_player(Player * p) {
	Main::audio->play(SND_ITEM, p->position->x);

	p->bombs += ammount;

	if(p->bombs > 9)
		p->bombs = 9;

	done = true;
}

void BombPowerUp::hit_npc(NPC * npc) {}

void BombPowerUp::draw(SDL_Surface * screen) {
	SDL_BlitSurface(surface, clip, screen, position);
}

void BombPowerUp::move(Level * level) {
	if(!level->is_on_bottom(position)) {
		done = true;
	}
}

void BombPowerUp::process() {}
