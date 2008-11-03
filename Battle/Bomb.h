#ifndef _BOMB_H
#define _BOMB_H

#include "Player.h"

class Bomb {
public:
	Bomb();
	Bomb(SDL_Surface * surface);
	~Bomb();

	SDL_Rect * position;
	SDL_Surface * sprite;
	SDL_Rect * clip[3];

	Player * owner;

	int speedy;

	int damage;

	int current_frame;

	static const int FRAME_COUNT;
	static const int FRAME_NORMAL;
	static const int FRAME_FLASH;
	static const int FRAME_EXPLOSION;
	
	void show(SDL_Surface * screen);
private:
	void set_clips();
};

#endif
