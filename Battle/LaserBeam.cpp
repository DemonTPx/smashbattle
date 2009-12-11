#include <SDL/SDL.h>

#include "Gameplay.h"
#include "AudioController.h"

#include "LaserBeam.h"

LaserBeam::LaserBeam() {
	start = Gameplay::instance->frame;

	position = new SDL_Rect();
	position->x = 0;
	position->y = -200;
	position->w = 4;
	position->h = 200 + WINDOW_HEIGHT;
	
}

LaserBeam::~LaserBeam() {
	delete position;
}

void LaserBeam::move(Level * level) {
	if(Gameplay::frame - start == 10) {
		SDL_Rect rect;

		rect.x = position->x - 34;
		rect.y = -200;
		rect.w = 66;
		rect.h = 200 + WINDOW_HEIGHT;

		level->damage_tiles(&rect, 25);
	}
}

void LaserBeam::process() {
	int frame;

	frame = Gameplay::instance->frame - start;

	if(frame == 1) {
		Main::audio->play(SND_LASER);
		position->x = target->position->x + (PLAYER_W / 2);
	}

	if(frame >= 28) {
		done = true;
	}
}

void LaserBeam::hit_player(Player * player) {
	if(player == owner)
		return;
	if(Gameplay::frame - start < 10)
		return;
	if(!player->is_hit) {
		player->hitpoints -= 25;
		player->is_hit = true;
		player->hit_start = Gameplay::frame;
	}
}

void LaserBeam::hit_npc(NPC * npc) {}

void LaserBeam::draw(SDL_Surface * screen) {
	SDL_Rect rect;
	int frame;
	Uint32 color;

	frame = Gameplay::instance->frame - start;

	if(frame < 0) return;

	if(frame == 12) {
		position->x -= 32;
		position->w = 66;
	}

	rect.x = position->x;
	rect.y = 0;
	rect.w = 4;
	rect.h = 0;

	color = 0xffffffff;
	
	// Preloading beam
	if(frame < 10) {
		rect.h = ((WINDOW_HEIGHT - TILE_W) / 10) * frame;

		SDL_FillRect(screen, &rect, color);
	} else if(frame < 12) {
		SDL_FillRect(screen, 0, color);
	} else if(frame < 28) {
		int offset;
		offset = (frame - 12) * 2;
		rect.h = WINDOW_HEIGHT;
		rect.w += 32 - offset;

		color = 0xffffffff - (0x00000002 * offset);

		SDL_FillRect(screen, &rect, color);

		rect.x += 32 + offset;
		
		SDL_FillRect(screen, &rect, color);
	}
}
 