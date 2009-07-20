#ifndef _PLAYERANIMATION_H
#define _PLAYERANIMATION_H

#include "Player.h"

class PlayerAnimation {
public:
	PlayerAnimation(int character);
	~PlayerAnimation();

	void set_character(int character);

	void move();
	void draw(SDL_Surface * screen);

	void set_sprite(int sprite);
	void cycle_sprite(int first, int last);
	void cycle_sprite_updown(int first, int last);

	SDL_Surface * sprites;
	SDL_Rect * position;

	int character;

	int momentumx;

	int direction;
	bool is_walking;
	bool is_running;
	bool is_duck;

	bool animate_in_place;

	int current_sprite;
	int cycle_direction;
	int distance_walked;
	int total_distance_walked;
};

#endif
