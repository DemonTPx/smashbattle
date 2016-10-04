#include "SDL2/SDL.h"

#include "Gameplay.h"
#include "GameplayObject.h"
#include "ShieldPowerUp.h"
#include "commands/CommandGeneratePowerup.h"
#include "Main.h"

ShieldPowerUp::ShieldPowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position, Main &main) : GameplayObject(main), main_(main) {
	clip->x = 108;
	clip->y = 0;
	this->surface = surface;
	this->clip = clip;
	this->position = position;
	is_powerup = true;
}

ShieldPowerUp::~ShieldPowerUp() {
	delete clip;
	delete position;
}

void ShieldPowerUp::hit_player(Player * p) {
	main_.audio->play(SND_ITEM, p->position->x);

	p->is_shielded = true;
	p->shield_start = main_.gameplay().frame;

	done = true;
}

void ShieldPowerUp::hit_npc(NPC * npc) {}

void ShieldPowerUp::draw_impl(SDL_Surface * screen, int frames_processed) {
	SDL_BlitSurface(surface, clip, screen, position);
}

void ShieldPowerUp::move(Level * level) {
	if(!level->is_on_bottom(position)) {
		done = true;
	}
}

void ShieldPowerUp::process() {}

void ShieldPowerUp::copyTo(network::CommandGeneratePowerup &powerup)
{
	GameplayObject::copyTo(powerup);

	powerup.data.type = network::CommandGeneratePowerup::PowerUps::TypeShield;
}
