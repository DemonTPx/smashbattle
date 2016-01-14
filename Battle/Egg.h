#pragma once

#include "Bomb.h"
#include "Player.h"

#define EGG_W 12
#define EGG_H 16

class Egg : public Bomb {
public:
	Egg(SDL_Surface * surface, Main &main);

	void process();
protected:
	void set_clips();

	Main &main_;
};
