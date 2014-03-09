#pragma once

#include "GameplayObject.h"

class Airstrike : public GameplayObject {
public:
	Airstrike(Main &main);
	~Airstrike();

	virtual void move(Level * level);
	virtual void process();

	virtual void hit_player(Player * player);
	virtual void hit_npc(NPC * npc);

	virtual void draw(SDL_Surface * screen, int frames_processed = 0);

	static const int DELAY;

	int start;

	Player * owner;

	Main &main_;
};
