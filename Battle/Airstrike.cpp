#include <SDL/SDL.h>

#include "Gameplay.h"
#include "AudioController.h"
#include "Bomb.h"

#include "Airstrike.h"

const int Airstrike::DELAY = 120;

Airstrike::Airstrike() {
	start = Gameplay::instance->frame;

	position = new SDL_Rect();
	position->x = 0;
	position->y = -200;
	position->w = 0;
	position->h = 0;
}

Airstrike::~Airstrike() {
	delete position;
}

void Airstrike::move(Level * level) {}

void Airstrike::process() {
	if(Gameplay::instance->frame - start == 1) {
		Main::audio->play(SND_AIRSTRIKE);
	}
	if(Gameplay::instance->frame - start == DELAY) {
		// Generate bombs!

		Bomb * b;
		int x_interval, x_correct;
		int bombs;

		bombs = 10;

		x_interval = WINDOW_WIDTH / bombs;
		x_correct = -(BOMB_W / 2);

		for(int i = 0; i < bombs; i++) {
			b = new Bomb(Main::graphics->bombs);
			b->position->x = (x_interval * (i + 1)) + x_correct;
			b->position->y = -(rand() % 64);
			b->speedy = 30;
			b->time = 900;
			b->damage = Player::BOMBPOWERCLASSES[owner->bombpowerclass].damage;
			b->owner = owner;
			b->frame_start = Gameplay::instance->frame;
			b->frame_change_start = Gameplay::frame;
			b->frame_change_count = 12;
			b->hit_on_impact = true;
			b->current_frame = Bomb::FRAME_STRIKE_NORMAL;

			Gameplay::instance->add_object(b);
		}

		done = true;
	}
}

void Airstrike::hit_player(Player * player) {}

void Airstrike::hit_npc(NPC * npc) {}

void Airstrike::draw(SDL_Surface * screen) {}

