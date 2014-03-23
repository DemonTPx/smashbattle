#include "SDL/SDL.h"

#include "GameplayObject.h"
#include "BombPowerUp.h"
#include "commands/CommandGeneratePowerup.h"
#include "Main.h"

BombPowerUp::BombPowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position, int ammount, Main &main) : GameplayObject(main), main_(main) {
	clip->x = 16;
	clip->y = 0;
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
	main_.audio->play(SND_ITEM, p->position->x);

	p->bombs += ammount;

	if(p->bombs > 9)
		p->bombs = 9;

	done = true;
}

void BombPowerUp::hit_npc(NPC * npc) {}

void BombPowerUp::draw_impl(SDL_Surface * screen, int frames_processed) {
	SDL_BlitSurface(surface, clip, screen, position);
}

void BombPowerUp::move(Level * level) {
	if(!level->is_on_bottom(position)) {
		done = true;
	}
}

void BombPowerUp::process() {}

void BombPowerUp::copyTo(network::CommandGeneratePowerup &powerup)
{
	GameplayObject::copyTo(powerup);

	powerup.data.type = network::CommandGeneratePowerup::PowerUps::TypeBomb;
	powerup.data.param = this->ammount;
}