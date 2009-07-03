#include "SDL/SDL.h"

#include "Block.h"

Block::Block() {
	x = 0;
	y = 0;
	w = 20;
	h = 20;
	xSpeed = 0;
	ySpeed = 0;
	surf = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32,
		0, 0, 0, 0);
	SDL_FillRect(surf, &surf->clip_rect, 0xff0000);
}

Block::~Block() {
	SDL_FreeSurface(surf);
}

void Block::handle_input(SDL_Event * event) {
	if(event->type == SDL_KEYDOWN) {
		switch(event->key.keysym.sym) {
			case SDLK_UP: ySpeed -= 1; break;
			case SDLK_DOWN: ySpeed += 1; break;
			case SDLK_LEFT: xSpeed -= 1; break;
			case SDLK_RIGHT: xSpeed += 1; break;
		}
	}
}

void Block::move() {
	x += xSpeed;
	if((x < 0) || (x > 790)) {
		x -= xSpeed;
		xSpeed = -xSpeed;
	}

	y += ySpeed;
	if((y < 0) || (y > 590)) {
		y -= ySpeed;
		ySpeed = -ySpeed;
	}
}

void Block::show(SDL_Surface * screen) {
	SDL_Rect offset;
	offset.x = x;
	offset.y = y;

	SDL_BlitSurface(surf, NULL, screen, &offset);
}

SDL_Rect * Block::get_rect() {
	SDL_Rect * rect;
	rect = new SDL_Rect();
	rect->x = x;
	rect->y = y;
	rect->w = w;
	rect->h = h;
	return rect;
}