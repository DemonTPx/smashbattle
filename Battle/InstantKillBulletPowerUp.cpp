#include "SDL/SDL.h"

#include "GameplayObject.h"
#include "InstantKillBulletPowerUp.h"
#include "commands/CommandGeneratePowerup.h"
#include "Main.h"

InstantKillBulletPowerUp::InstantKillBulletPowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position, int ammo, Main &main) : GameplayObject(main), main_(main) {
	clip->x = 72;
	clip->y = 0;
	this->surface = surface;
	this->clip = clip;
	this->position = position;
	this->ammo = ammo;
	is_powerup = true;
}

InstantKillBulletPowerUp::~InstantKillBulletPowerUp() {
	delete clip;
	delete position;
}

void InstantKillBulletPowerUp::hit_player(Player * p) {
	main_.audio->play(SND_ITEM, p->position->x);

	p->instantkillbullets += ammo;

	if(p->instantkillbullets > 99)
		p->instantkillbullets = 99;

	done = true;
}

void InstantKillBulletPowerUp::hit_npc(NPC * npc) {}

void InstantKillBulletPowerUp::draw_impl(SDL_Surface * screen, int frames_processed) {
	SDL_BlitSurface(surface, clip, screen, position);
}

void InstantKillBulletPowerUp::move(Level * level) {
	if(!level->is_on_bottom(position)) {
		done = true;
	}
}

void InstantKillBulletPowerUp::process() {}

void InstantKillBulletPowerUp::copyTo(network::CommandGeneratePowerup &powerup)
{
	GameplayObject::copyTo(powerup);

	powerup.data.type = network::CommandGeneratePowerup::PowerUps::TypeInstantKillBullet;
	powerup.data.param = this->ammo;
}
