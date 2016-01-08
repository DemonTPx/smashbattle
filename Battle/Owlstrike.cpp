#include <SDL/SDL.h>

#include "Gameplay.h"
#include "AudioController.h"
#include "Bomb.h"
#include "Owl.h"
#include "Main.h"
#include "Owlstrike.h"

const int Owlstrike::DELAY = 120;

Owlstrike::Owlstrike(Main &main) : GameplayObject(main), main_(main) {
	start = main_.gameplay().frame;

	position = new SDL_Rect();
	position->x = 0;
	position->y = -200;
	position->w = 0;
	position->h = 0;
}

Owlstrike::~Owlstrike() {
	delete position;
}

void Owlstrike::move(Level * level) {}

void Owlstrike::process() {
	if(main_.gameplay().frame - start == 1) {
		main_.audio->play(SND_OWLSTRIKE);
	}
	if(main_.gameplay().frame - start == DELAY) {
		// Generate le owl
		Owl * owl = new Owl(main_.graphics->owl, main_);
		owl->owner = owner;
		main_.gameplay().add_object(owl);

		// Generate bombs!
		/*
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
		*/

		done = true;
	}
}

void Owlstrike::hit_player(Player * player) {}

void Owlstrike::hit_npc(NPC * npc) {}

void Owlstrike::draw_impl(SDL_Surface * screen, int frames_processed) {}

