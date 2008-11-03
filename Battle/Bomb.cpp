#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"

#include "Bomb.h"

const int Bomb::FRAME_COUNT = 3;
const int Bomb::FRAME_NORMAL = 0;
const int Bomb::FRAME_FLASH = 1;
const int Bomb::FRAME_EXPLOSION = 2;

Bomb::Bomb() {
	damage = 0;
	speedy = 0;

	position = new SDL_Rect();
}

Bomb::Bomb(SDL_Surface * surface) {
	damage = 0;
	speedy = 0;

	sprite = surface;

	position = new SDL_Rect();
	position->x = 0;
	position->y = 0;
	position->w = 16;
	position->h = 16;

	current_frame = 0;

	set_clips();
}

Bomb::~Bomb() {
	delete position;
	delete clip[FRAME_NORMAL];
	delete clip[FRAME_FLASH];
	delete clip[FRAME_EXPLOSION];
}

void Bomb::show(SDL_Surface * screen) {
	SDL_Rect rect;

	rect.x = position->x;
	rect.y = position->y;

	SDL_BlitSurface(sprite, clip[current_frame], screen, &rect);

	// If the bomb is going out the side of the screen, we want it to
	// appear on the other side.
	if(position->x >= screen->w - clip[current_frame]->w) {
		rect.x = position->x - screen->w;
		rect.y = position->y;
		SDL_BlitSurface(sprite, clip[current_frame], screen, &rect);
	}
	if(position->x <= 0) {
		rect.x = position->x + screen->w;
		rect.y = position->y;
		SDL_BlitSurface(sprite, clip[current_frame], screen, &rect);
	}
}

void Bomb::set_clips() {
	clip[FRAME_NORMAL] = new SDL_Rect();
	clip[FRAME_NORMAL]->x = 0;
	clip[FRAME_NORMAL]->y = 0;
	clip[FRAME_NORMAL]->w = 16;
	clip[FRAME_NORMAL]->h = 16;
	
	clip[FRAME_FLASH] = new SDL_Rect();
	clip[FRAME_FLASH]->x = 16;
	clip[FRAME_FLASH]->y = 0;
	clip[FRAME_FLASH]->w = 16;
	clip[FRAME_FLASH]->h = 16;
	
	clip[FRAME_EXPLOSION] = new SDL_Rect();
	clip[FRAME_EXPLOSION]->x = 0;
	clip[FRAME_EXPLOSION]->y = 16;
	clip[FRAME_EXPLOSION]->w = 86;
	clip[FRAME_EXPLOSION]->h = 68;
}
