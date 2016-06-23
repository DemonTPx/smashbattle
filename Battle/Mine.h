#pragma once

#include "Bomb.h"
#include "Player.h"

#define MINE_W 6
#define MINE_H 4

class Mine : public Bomb {
public:
	Mine(SDL_Surface * surface, Main &main);

	int flash_interval;
	int flash_length;
	int flash_start;

	void process();
protected:
    virtual void set_clips();

	Main &main_;
};
