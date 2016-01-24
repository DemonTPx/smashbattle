#include <SDL/SDL.h>

#include "Main.h"
#include "Gameplay.h"
#include "Airstrike.h"

#include "AirstrikePowerUp.h"
#include "commands/CommandGeneratePowerup.h"

AirstrikePowerUp::AirstrikePowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position, Main &main) : GameplayObject(main), main_(main) {
	clip->x = 90;
	clip->y = 0;
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
	main_.audio->play(SND_ITEM, p->position->x);

	shoot_airstrike(p, main_);

	done = true;
}

void AirstrikePowerUp::shoot_airstrike(Player * p, Main &main) {
	Airstrike * as;

	as = new Airstrike(main);
	as->owner = p;

	main.gameplay().add_object(as);
}

void AirstrikePowerUp::hit_npc(NPC * npc) {}

void AirstrikePowerUp::draw_impl(SDL_Surface * screen, int frames_processed) {
	SDL_BlitSurface(surface, clip, screen, position);
}

void AirstrikePowerUp::move(Level * level) {
	if(!level->is_on_bottom(position)) {
		done = true;
	}
}

void AirstrikePowerUp::process() {}

void AirstrikePowerUp::copyTo(network::CommandGeneratePowerup &powerup)
{
	GameplayObject::copyTo(powerup);

	powerup.data.type = network::CommandGeneratePowerup::PowerUps::TypeAirstrike;
}
