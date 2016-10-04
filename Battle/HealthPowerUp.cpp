#include "SDL2/SDL.h"

#include "GameplayObject.h"
#include "Main.h"
#include "HealthPowerUp.h"
#include "commands/CommandGeneratePowerup.h"

HealthPowerUp::HealthPowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position, int hp, Main &main) : GameplayObject(main), main_(main) {
	clip->x = 0;
	clip->y = 0;
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
	main_.audio->play(SND_ITEM, p->position->x);

	p->hitpoints += hp;

	if(p->hitpoints > 100)
		p->hitpoints = 100;

	done = true;
}

void HealthPowerUp::hit_npc(NPC * npc) {}

void HealthPowerUp::draw_impl(SDL_Surface * screen, int frames_processed) {
	SDL_BlitSurface(surface, clip, screen, position);
}

void HealthPowerUp::move(Level * level) {
	if(!level->is_on_bottom(position)) {
		done = true;
	}
}

void HealthPowerUp::process() {}

void HealthPowerUp::copyTo(network::CommandGeneratePowerup &powerup)
{
	GameplayObject::copyTo(powerup);

	powerup.data.type = network::CommandGeneratePowerup::PowerUps::TypeHealth;
	powerup.data.param = this->hp;
}
