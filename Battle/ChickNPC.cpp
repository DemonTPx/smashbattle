#include "SDL/SDL.h"

#include "Gameplay.h"

#include "ChickNPC.h"

ChickNPC::ChickNPC() : NPC() {
	is_stationary = false;

	frame_w = 24;
	frame_h = 26;

	frames = 8;

	frame_idle = 0;
	
	frame_walk_first = 1;
	frame_walk_last = 3;

	frame_jump_first = 4;
	frame_jump_last= 5;
	
	frame_brake_first = 6;
	frame_brake_last = 6;

	frame_dead = 7;

	sprites = Main::graphics->npc_chick;
	
	position->w = frame_w;
	position->h = frame_h;

	last_position->w = frame_w;
	last_position->h = frame_h;

	max_speed = 20;
	jump_height = 0;

	bounce_weight = 10;

	hitpoints = 1;

	move_direction = 1;
}

ChickNPC::~ChickNPC() {
	delete position;
	delete last_position;
}

void ChickNPC::process() {
	NPC::process();

	SDL_Rect rect;

	if(!is_falling && position->y > 360) {
		rect.w = 1;
		rect.h = position->h;
		if(move_direction == -1) {
			rect.x = position->x - 8;
			rect.y = position->y;
		} else {
			rect.x = position->x + position->w + 8;
			rect.y = position->y;
		}

		if(!Gameplay::instance->level->is_on_bottom(&rect)) {
			move_direction = -move_direction;
		}
	}
}

void ChickNPC::reset() {
	if(move_direction == -1) {
		set_sprite(frame_idle + frames);
	} else {
		set_sprite(frame_idle);
	}
}

void ChickNPC::hit_player_side(Player * p) {
	if(p->damage(10)) {
		Main::audio->play(SND_HIT, position->x);
	}
}

void ChickNPC::hit_player_top_bottom(Player * p) {}
