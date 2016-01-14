#include "SDL/SDL.h"

#include "Gameplay.h"
#include "Egg.h"
#include "Main.h"

Egg::Egg(SDL_Surface * surface, Main &main) : Bomb(surface, main), main_(main) {
	position->w = EGG_W;
	position->h = EGG_H;

	set_clips();
}

void Egg::process() {
	Bomb::process();
}

void Egg::set_clips() {
	clip[FRAME_NORMAL] = new SDL_Rect();
	clip[FRAME_NORMAL]->x = 60;
	clip[FRAME_NORMAL]->y = 0;
	clip[FRAME_NORMAL]->w = EGG_W;
	clip[FRAME_NORMAL]->h = EGG_H;

	clip[FRAME_FLASH] = new SDL_Rect();
	clip[FRAME_FLASH]->x = 60 + EGG_W;
	clip[FRAME_FLASH]->y = 0;
	clip[FRAME_FLASH]->w = EGG_W;
	clip[FRAME_FLASH]->h = EGG_H;

	clip[FRAME_STRIKE_NORMAL] = new SDL_Rect();
	clip[FRAME_STRIKE_NORMAL]->x = 60;
	clip[FRAME_STRIKE_NORMAL]->y = 0;
	clip[FRAME_STRIKE_NORMAL]->w = EGG_W;
	clip[FRAME_STRIKE_NORMAL]->h = EGG_H;

	clip[FRAME_STRIKE_FLASH] = new SDL_Rect();
	clip[FRAME_STRIKE_FLASH]->x = 60 + EGG_W;
	clip[FRAME_STRIKE_FLASH]->y = 0;
	clip[FRAME_STRIKE_FLASH]->w = EGG_W;
	clip[FRAME_STRIKE_FLASH]->h = EGG_H;

	explosion_offset_x = (clip[FRAME_NORMAL]->w - clip[FRAME_EXPLOSION]->w) / 2;
	explosion_offset_y = -clip[FRAME_EXPLOSION]->h + clip[FRAME_NORMAL]->h + 12;
}
