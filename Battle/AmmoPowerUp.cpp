#include "SDL/SDL.h"

#include "GameplayObject.h"
#include "AmmoPowerUp.h"
#include "commands/CommandGeneratePowerup.h"

AmmoPowerUp::AmmoPowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position, int ammo, Main &main) : main_(main) {
	clip->x = 32;
	clip->y = 0;
	this->surface = surface;
	this->clip = clip;
	this->position = position;
	this->ammo = ammo;
	is_powerup = true;
}

AmmoPowerUp::~AmmoPowerUp() {
	delete clip;
	delete position;
}

void AmmoPowerUp::hit_player(Player * p) {
	main_.audio->play(SND_ITEM, p->position->x);

	p->bullets += ammo;

	if(p->bullets > 99)
		p->bullets = 99;

	done = true;
}

void AmmoPowerUp::hit_npc(NPC * npc) {}

void AmmoPowerUp::draw(SDL_Surface * screen, int frames_processed) {
	SDL_BlitSurface(surface, clip, screen, position);
}

void AmmoPowerUp::move(Level * level) {
	if(!level->is_on_bottom(position)) {
		done = true;
	}
}

void AmmoPowerUp::process() {}

void AmmoPowerUp::copyTo(network::CommandGeneratePowerup &powerup)
{
	GameplayObject::copyTo(powerup);

	powerup.data.type = network::CommandGeneratePowerup::PowerUps::TypeAmmo;
	powerup.data.param = this->ammo;
}