#include <SDL/SDL.h>

#include "Gameplay.h"
#include "AudioController.h"
#include "Bomb.h"
#include "Main.h"
#include "Airstrike.h"

const int Airstrike::DELAY = 120;

Airstrike::Airstrike(Main &main) : GameplayObject(main), main_(main) {
	start = main_.gameplay().frame;

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
	if(main_.gameplay().frame - start == 1) {
		main_.audio->play(SND_AIRSTRIKE);
	}
	if(main_.gameplay().frame - start == DELAY) {
		// Generate bombs!

		Bomb * b;
		int x_interval, x_correct;
		int bombs;

		bombs = 10;

		x_interval = WINDOW_WIDTH / bombs;
		x_correct = -(BOMB_W / 2);

		for(int i = 0; i < bombs; i++) {
			b = new Bomb(main_.graphics->bombs, main_);
			b->position->x = (x_interval * (i + 1)) + x_correct;
			b->position->y = -(rand() % 64);
			b->speedy = 30;
			b->time = 900;
			b->damage = Player::BOMBPOWERCLASSES[owner->bombpowerclass].damage;
			b->owner = owner;
			b->frame_start = main_.gameplay().frame;
			b->frame_change_start = main_.gameplay().frame;
			b->frame_change_count = 12;
			b->hit_on_impact = true;
			b->current_frame = Bomb::FRAME_STRIKE_NORMAL;

			main_.gameplay().add_object(b);
		}

		done = true;
	}
}

void Airstrike::hit_player(Player * player) {}

void Airstrike::hit_npc(NPC * npc) {}

void Airstrike::draw_impl(SDL_Surface * screen, int frames_processed) {}

