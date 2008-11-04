#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"

#include "Bomb.h"

const int Bomb::FRAME_COUNT = 3;
const int Bomb::FRAME_NORMAL = 0;
const int Bomb::FRAME_FLASH = 1;
const int Bomb::FRAME_EXPLOSION = 2;
const int Bomb::FRAME_EXPLOSION2 = 3;

Bomb::Bomb() {
	damage = 0;
	speedy = 0;

	exploded = false;
	done = false;

	position = new SDL_Rect();
}

Bomb::Bomb(SDL_Surface * surface) {
	damage = 0;
	time = 180;
	speedy = 0;

	exploded = false;
	done = false;

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
	
	// Flicker explosion
	if(exploded) {
		if(current_frame != FRAME_EXPLOSION) {
			current_frame = FRAME_EXPLOSION;
			frame_change_start = 0;
			flicker_frame = 0;
		}
		flicker_frame++;
		if(flicker_frame >= 30)
			done = true;
		if(flicker_frame % 12 >= 6) return;
	}

	if(current_frame == FRAME_EXPLOSION2) return;
	if(current_frame == FRAME_EXPLOSION) {
		rect.x += explosion_offset_x;
		rect.y += explosion_offset_y;
	}

	SDL_BlitSurface(sprite, clip[current_frame], screen, &rect);

	// If the bomb is going out the side of the screen, we want it to
	// appear on the other side.
	if(position->x >= screen->w - clip[current_frame]->w) {
		rect.x = rect.x - screen->w;
		rect.y = rect.y;
		SDL_BlitSurface(sprite, clip[current_frame], screen, &rect);
	}
	if(position->x <= 0) {
		rect.x = rect.x + screen->w;
		rect.y = rect.y;
		SDL_BlitSurface(sprite, clip[current_frame], screen, &rect);
	}
}

SDL_Rect * Bomb::get_damage_rect() {
	SDL_Rect * rect;

	rect = new SDL_Rect();
	rect->x = position->x + explosion_offset_x;
	rect->y = position->y + explosion_offset_y;
	rect->w = clip[FRAME_EXPLOSION]->w;
	rect->h = clip[FRAME_EXPLOSION]->h;
	return rect;
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

	explosion_offset_x = (clip[FRAME_NORMAL]->w - clip[FRAME_EXPLOSION]->w) / 2;
	explosion_offset_y = -clip[FRAME_EXPLOSION]->h + clip[FRAME_NORMAL]->h;
}
