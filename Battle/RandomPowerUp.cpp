#include "SDL/SDL.h"

#include "Gameplay.h"
#include "GameplayObject.h"
#include "HealthPowerUp.h"
#include "AmmoPowerUp.h"
#include "DoubleDamagePowerUp.h"
#include "InstantKillBulletPowerUp.h"
#include "BombPowerUp.h"
#include "MinePowerUp.h"
#include "AirstrikePowerUp.h"
#include "LaserBeamPowerUp.h"
#include "ShieldPowerUp.h"
#include "RandomPowerUp.h"
#include "commands/CommandGeneratePowerup.h"

const int RandomPowerUp::CYCLE_COUNT = 7;
const int RandomPowerUp::CYCLE_X[CYCLE_COUNT] = {48, 80, 64, 112, 16, 128, 96};
const int RandomPowerUp::CYCLE_DELAY = 20;

#define RPU_DOUBLEDAMAGE 0
#define RPU_AIRSTRIKE 1
#define RPU_INSTANTKILL 2
#define RPU_LASER 3
#define RPU_BOMB 4
#define RPU_MINE 5
#define RPU_SHIELD 6

RandomPowerUp::RandomPowerUp(SDL_Surface * surface, SDL_Rect * position, Main &main) : main_(main) {
	this->surface = surface;
	this->position = position;

	cycle = rand() % CYCLE_COUNT;

	frame_counter = 0;

	clip = new SDL_Rect();
	clip->x = CYCLE_X[cycle];
	clip->y = 0;
	clip->w = 16;
	clip->h = 16;

	is_powerup = true;
}

RandomPowerUp::~RandomPowerUp() {
	delete clip;
	delete position;
}

void RandomPowerUp::hit_player(Player * p) {
	main_.audio->play(SND_ITEM, p->position->x);

	switch(cycle) {
		case RPU_DOUBLEDAMAGE:
			p->doubledamagebullets += 5;
			if(p->doubledamagebullets > 99)
				p->doubledamagebullets = 99;
			break;
		case RPU_INSTANTKILL:
			p->instantkillbullets += 1;
			if(p->instantkillbullets > 99)
				p->instantkillbullets = 99;
			break;
		case RPU_BOMB:
			p->bombs += 1;
			if(p->bombs > 9)
				p->bombs = 9;
			break;
		case RPU_MINE:
			p->mines += 1;
			if(p->mines > 9)
				p->mines = 9;
			break;
		case RPU_AIRSTRIKE:
			AirstrikePowerUp::shoot_airstrike(p, main_);
			break;
		case RPU_LASER:
			LaserBeamPowerUp::shoot_laserbeam(p, main_);
			break;
		case RPU_SHIELD:
			p->is_shielded = true;
			p->shield_start = Gameplay::frame;
			break;
	}

	done = true;
}

void RandomPowerUp::hit_npc(NPC * npc) {}

void RandomPowerUp::draw(SDL_Surface * screen, int frames_processed) {
	frame_counter += frames_processed;
	SDL_BlitSurface(surface, clip, screen, position);
}

void RandomPowerUp::move(Level * level) {
	if(!level->is_on_bottom(position)) {
		done = true;
	}
}

void RandomPowerUp::process() {
	if(frame_counter >= CYCLE_DELAY) {
		int last;

		last = cycle;
		while(cycle == last) {
			cycle = rand() % CYCLE_COUNT;
		}

		clip->x = CYCLE_X[cycle];
		frame_counter = 0;
	}
}

void RandomPowerUp::copyTo(network::CommandGeneratePowerup &powerup)
{
	GameplayObject::copyTo(powerup);

	powerup.data.type = network::CommandGeneratePowerup::PowerUps::TypeRandom;
}