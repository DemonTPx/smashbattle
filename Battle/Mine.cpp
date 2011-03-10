#include "SDL/SDL.h"

#include "Gameplay.h"
#include "Mine.h"

Mine::Mine(SDL_Surface * surface) : Bomb(surface) {
	flash_interval = 50;
	flash_length = 10;
	flash_start = Gameplay::frame;

	position->w = MINE_W;
	position->h = MINE_H;

	set_clips();
}

void Mine::process() {
	if(!exploded) {
		// Animate mine
		if(current_frame == FRAME_NORMAL) {
			if(Gameplay::frame - flash_start >= flash_interval) {
				current_frame = FRAME_FLASH;
				Main::audio->play(SND_BLIP, position->x);
			}
		} else {
			if(Gameplay::frame - flash_start - flash_interval >= flash_length) {
				current_frame = FRAME_NORMAL;
				flash_start = Gameplay::frame;
			}
		}
	}
}

void Mine::set_clips() {
	// Delete some inherited stuff
	delete clip[FRAME_NORMAL];
	delete clip[FRAME_FLASH];

	clip[FRAME_NORMAL] = new SDL_Rect();
	clip[FRAME_NORMAL]->x = 48;
	clip[FRAME_NORMAL]->y = 0;
	clip[FRAME_NORMAL]->w = MINE_W;
	clip[FRAME_NORMAL]->h = MINE_H;

	clip[FRAME_FLASH] = new SDL_Rect();
	clip[FRAME_FLASH]->x = 48 + MINE_W;
	clip[FRAME_FLASH]->y = 0;
	clip[FRAME_FLASH]->w = MINE_W;
	clip[FRAME_FLASH]->h = MINE_H;

	explosion_offset_x = (clip[FRAME_NORMAL]->w - clip[FRAME_EXPLOSION]->w) / 2;
	explosion_offset_y = -clip[FRAME_EXPLOSION]->h + clip[FRAME_NORMAL]->h;
}
