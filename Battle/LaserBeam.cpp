#include <SDL2/SDL.h>

#include "Gameplay.h"
#include "AudioController.h"
#include "LaserBeam.h"
#include "Main.h"
#include "Color.h"

LaserBeam::LaserBeam(Main &main) : GameplayObject(main), main_(main) {
	start = main_.gameplay().frame;

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
	int frame;

	frame = main_.gameplay().frame - start;

	if (frame == 10) {
		SDL_Rect rect;

		rect.x = position->x - 34;
		rect.y = -200;
		rect.w = 66;
		rect.h = 200 + WINDOW_HEIGHT;

		level->damage_tiles(&rect, 25);
	}
	if (frame == 12) {
		position->x -= 32;
		position->w = 66;
	}
}

void LaserBeam::process() {
	int frame;

	frame = main_.gameplay().frame - start;

	if(frame == 1) {
		if(target->is_dead) {
			done = true;
			return;
		}

		main_.audio->play(SND_LASER);
		position->x = target->position->x + (PLAYER_W / 2);

		if(position->x > WINDOW_WIDTH)
			position->x -= WINDOW_WIDTH;
	}

	if(frame >= 28) {
		done = true;
	}
}

void LaserBeam::hit_player(Player * player) {
	if(player == owner)
		return;
	if(main_.gameplay().frame - start < 10)
		return;
	player->damage(25, owner, LASER);
}

void LaserBeam::hit_npc(NPC * npc) {
	npc->damage(25);
}

void LaserBeam::draw_impl(SDL_Surface * screen, int frames_processed) {
	SDL_Rect rect, rect_b;
	int frame;
	Uint32 color;

	frame = main_.gameplay().frame - start;

	if(frame < 0) return;

	rect.x = position->x;
	rect.y = 0;
	rect.w = 4;
	rect.h = 0;

	rect_b.x = rect.x;
	rect_b.y = rect.y;
	rect_b.w = rect.w;
	rect_b.h = rect.h;

	color = 0xffffffff;

	if (main_.no_sdl)
		return;
	
	// Preloading beam
	if(frame < 10) {
		rect.h = ((WINDOW_HEIGHT - TILE_W) / 10) * frame;

		SDL_FillRectColor(screen, &rect, color);

		if(rect.x < 0) {
			rect_b.h = rect.h;
			rect_b.x += WINDOW_WIDTH;
			SDL_FillRectColor(screen, &rect_b, color);
		}
		if(rect.x  + rect.w >= WINDOW_WIDTH) {
			rect_b.h = rect.h;
			rect_b.x -= WINDOW_WIDTH;
			SDL_FillRectColor(screen, &rect_b, color);
		}
	} else if(frame < 12) {
		SDL_FillRectColor(screen, 0, color);
	} else if(frame < 28) {
		int offset, width;
		offset = (frame - 12) * 2;
		width = 32 - offset;

		rect.h = WINDOW_HEIGHT;
		rect.w += width;

		color = 0xffffffff - (0x00000002 * offset);

		SDL_FillRectColor(screen, &rect, color);
		
		if(rect.x < 0) {
			rect_b.w = width;
			rect_b.h = rect.h;
			rect_b.x = rect.x + WINDOW_WIDTH;
			SDL_FillRectColor(screen, &rect_b, color);
		}
		if(rect.x  + width >= WINDOW_WIDTH) {
			rect_b.w = 32 - offset;
			rect_b.h = rect.h;
			rect_b.x = rect.x - WINDOW_WIDTH;
			SDL_FillRectColor(screen, &rect_b, color);
		}

		rect.x += 32 + offset;

		SDL_FillRectColor(screen, &rect, color);
		
		if(rect.x < 0) {
			rect_b.w = width;
			rect_b.h = rect.h;
			rect_b.x = rect.x + WINDOW_WIDTH;
			SDL_FillRectColor(screen, &rect_b, color);
		}
		if(rect.x  + width >= WINDOW_WIDTH) {
			rect_b.w = width;
			rect_b.h = rect.h;
			rect_b.x = rect.x - WINDOW_WIDTH;
			SDL_FillRectColor(screen, &rect_b, color);
		}
	}
}
