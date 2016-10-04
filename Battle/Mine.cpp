#include "SDL2/SDL.h"

#include "Gameplay.h"
#include "Mine.h"
#include "Main.h"

Mine::Mine(SDL_Surface * surface, Main &main) : Bomb(surface, main), main_(main) {
	flash_interval = 50;
	flash_length = 10;
	flash_start = main_.gameplay().frame;

	position->w = MINE_W;
	position->h = MINE_H;

	set_clips();
}

void Mine::process() {
	if(!exploded) {
		// Animate mine
		if(current_frame == FRAME_NORMAL) {
			if(main_.gameplay().frame - flash_start >= flash_interval) {
				current_frame = FRAME_FLASH;
				main_.audio->play(SND_BLIP, position->x);
			}
		} else {
			if(main_.gameplay().frame - flash_start - flash_interval >= flash_length) {
				current_frame = FRAME_NORMAL;
				flash_start = main_.gameplay().frame;
			}
		}
	}
}

void Mine::set_clips() {
	// Delete some inherited stuff
	delete clip[FRAME_NORMAL];
	delete clip[FRAME_FLASH];

	clip[FRAME_NORMAL] = new SDL_Rect();
	clip[FRAME_NORMAL]->x = 72;
	clip[FRAME_NORMAL]->y = 12;
	clip[FRAME_NORMAL]->w = MINE_W;
	clip[FRAME_NORMAL]->h = MINE_H;

	clip[FRAME_FLASH] = new SDL_Rect();
	clip[FRAME_FLASH]->x = 72 + MINE_W;
	clip[FRAME_FLASH]->y = 12;
	clip[FRAME_FLASH]->w = MINE_W;
	clip[FRAME_FLASH]->h = MINE_H;

	explosion_offset_x = (clip[FRAME_NORMAL]->w - clip[FRAME_EXPLOSION]->w) / 2;
	explosion_offset_y = -clip[FRAME_EXPLOSION]->h + clip[FRAME_NORMAL]->h + 12;
}
