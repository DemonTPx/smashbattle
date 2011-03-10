#ifndef _MINE_H
#define _MINE_H

#include "Bomb.h"
#include "Player.h"

#define MINE_W 6
#define MINE_H 4

class Mine : public Bomb {
public:
	Mine(SDL_Surface * surface);

	int flash_interval;
	int flash_length;
	int flash_start;

	void process();
protected:
	void set_clips();
};

#endif
