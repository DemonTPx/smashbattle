#include "SDL/SDL.h"

#include "Gameplay.h"

#include "Projectile.h"

#include "GatlingNPC.h"

#define GATLING_L1	0
#define GATLING_L2	1
#define GATLING_R1	2
#define GATLING_R2	3

GatlingNPC::GatlingNPC() : NPC() {
	is_stationary = true;

	frame_w = 64;
	frame_h = 28;

	frames = 2;

	frame_idle = GATLING_L1;

	frame_dead = 0;

	sprites = Main::graphics->npc_gatling;
	
	position->w = frame_w;
	position->h = frame_h;

	last_position->w = frame_w;
	last_position->h = frame_h;

	hitpoints = 50;

	frame_first = GATLING_L1;
	frame_last = GATLING_L2;
}

void GatlingNPC::process() {
	if(!is_dead && hitpoints <= 0) {
		is_dead = true;
		dead_start = Gameplay::frame;
	}
	if(is_dead && (Gameplay::frame - dead_start >= 30)) {
		done = true;
	}

	SDL_Rect * player;
	range.x = position->x - 500;
	if(range.x < 0) range.x += WINDOW_WIDTH;
	range.y = position->y + 2;
	range.h = 4;
	range.w = 500;

	player = Gameplay::instance->players->at(0)->position;

	if(Gameplay::instance->is_intersecting(player, &range)) {
		shoot();
	}
}

void GatlingNPC::reset() {
	if(move_direction == -1) {
		frame_first = GATLING_L1;
		frame_last = GATLING_L2;
		set_sprite(GATLING_L1);
	} else {
		frame_first = GATLING_R1;
		frame_last = GATLING_R2;
		set_sprite(GATLING_R1);
	}
}

void GatlingNPC::shoot() {
	if(Gameplay::frame - shoot_start < 6) return;

	Projectile * pr;
	SDL_Rect * clip_weapon;

	shoot_start = Gameplay::frame;

	clip_weapon = new SDL_Rect();
	clip_weapon->x = 0;
	clip_weapon->y = 0;
	clip_weapon->w = 8;
	clip_weapon->h = 8;

	pr = new Projectile(Main::graphics->weapons, clip_weapon);
	pr->owner = NULL;

	pr->damage = 10;
	
	pr->speedx = 0;
	pr->speedy = 0;
	pr->max_distance = 450;

	switch(current_sprite) {
		case GATLING_L1:
		case GATLING_L2:
			pr->speedx = -10;
			pr->position->x = position->x - 8;
			pr->position->y = position->y + 2 + (rand() % 4);
			break;
		case GATLING_R1:
		case GATLING_R2:
			pr->speedx = 10;
			pr->position->x = position->x + position->w;
			pr->position->y = position->y + 2 + (rand() % 4);
			break;
	}
	
	Gameplay::instance->add_object(pr);
	
	cycle_sprite_updown(frame_first, frame_last);
}

void GatlingNPC::hit_player_side(Player * p) {
	if(p->damage(10)) {
		Main::audio->play(SND_HIT, position->x);
	}
}

void GatlingNPC::hit_player_top_bottom(Player * p) {
	if(p->damage(10)) {
		Main::audio->play(SND_HIT, position->x);
	}
}
