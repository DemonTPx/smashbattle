#include <SDL2/SDL.h>

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
	if(main_.gameplay().frame - start >= DELAY) {
		// Generate le owl
		Owl * owl = new Owl(main_.graphics->owl, main_);
		owl->owner = owner;
		main_.gameplay().add_object(owl);

		done = true;
	}
}

void Owlstrike::hit_player(Player * player) {}

void Owlstrike::hit_npc(NPC * npc) {}

void Owlstrike::draw_impl(SDL_Surface * screen, int frames_processed) {}

