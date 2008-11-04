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
	int explosion_offset_x, explosion_offset_y;

	Player * owner;

	int speedy;

	bool exploded;
	bool done;

	int damage;
	int time;

	int current_frame;
	int frame_start;
	int frame_change_start;
	int frame_change_count;
	int frame_explode;
	int flicker_frame;

	static const int FRAME_COUNT;
	static const int FRAME_NORMAL;
	static const int FRAME_FLASH;
	static const int FRAME_EXPLOSION;
	static const int FRAME_EXPLOSION2;
	
	void show(SDL_Surface * screen);
	SDL_Rect * get_damage_rect();
private:
	void set_clips();
};

#endif
