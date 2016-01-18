#pragma once

#include "Bomb.h"
#include "Player.h"

#define EGG_W 12
#define EGG_H 16

class Egg : public Bomb {
public:
	Egg(SDL_Surface * surface, Main &main);

	virtual void hit_player(Player * player);

	virtual void explode();
protected:
	void set_clips();

	Main &main_;
};
