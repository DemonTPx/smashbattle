#include <SDL/SDL.h>

#include "Main.h"
#include "Gameplay.h"
#include "Owlstrike.h"

#include "OwlstrikePowerUp.h"
#include "commands/CommandGeneratePowerup.h"

OwlstrikePowerUp::OwlstrikePowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position, Main &main) : GameplayObject(main), main_(main) {
	clip->x = 144;
	clip->y = 0;
	this->surface = surface;
	this->clip = clip;
	this->position = position;
	is_powerup = true;
}

OwlstrikePowerUp::~OwlstrikePowerUp() {
	delete clip;
	delete position;
}

void OwlstrikePowerUp::hit_player(Player * p) {
	main_.audio->play(SND_ITEM, p->position->x);

	shoot_owlstrike(p, main_);

	done = true;
}

void OwlstrikePowerUp::shoot_owlstrike(Player * p, Main &main) {
	Owlstrike *os;

	os = new Owlstrike(main);
	os->owner = p;

	main.gameplay().add_object(os);
}

void OwlstrikePowerUp::hit_npc(NPC * npc) {}

void OwlstrikePowerUp::draw_impl(SDL_Surface * screen, int frames_processed) {
	SDL_BlitSurface(surface, clip, screen, position);
}

void OwlstrikePowerUp::move(Level * level) {
	if(!level->is_on_bottom(position)) {
		done = true;
	}
}

void OwlstrikePowerUp::process() {}

void OwlstrikePowerUp::copyTo(network::CommandGeneratePowerup &powerup)
{
	GameplayObject::copyTo(powerup);

	powerup.data.type = network::CommandGeneratePowerup::PowerUps::TypeOwlstrike;
}
