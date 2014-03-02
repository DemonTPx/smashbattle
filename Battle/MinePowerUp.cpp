#include "SDL/SDL.h"

#include "GameplayObject.h"
#include "MinePowerUp.h"
#include "commands/CommandGeneratePowerup.h"

MinePowerUp::MinePowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position, int ammount) {
	clip->x = 128;
	clip->y = 0;
	this->surface = surface;
	this->clip = clip;
	this->position = position;
	this->ammount = ammount;
	is_powerup = true;
}

MinePowerUp::~MinePowerUp() {
	delete clip;
	delete position;
}

void MinePowerUp::hit_player(Player * p) {
	Main::audio->play(SND_ITEM, p->position->x);

	p->mines += ammount;

	if(p->mines > 9)
		p->mines = 9;

	done = true;
}

void MinePowerUp::hit_npc(NPC * npc) {}

void MinePowerUp::draw(SDL_Surface * screen, int frames_processed) {
	SDL_BlitSurface(surface, clip, screen, position);
}

void MinePowerUp::move(Level * level) {
	if(!level->is_on_bottom(position)) {
		done = true;
	}
}

void MinePowerUp::process() {}

void MinePowerUp::copyTo(network::CommandGeneratePowerup &powerup)
{
	GameplayObject::copyTo(powerup);

	powerup.data.type = network::CommandGeneratePowerup::PowerUps::TypeMine;
	powerup.data.param = this->ammount;
}