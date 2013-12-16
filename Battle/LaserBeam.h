#pragma once

#include "GameplayObject.h"

class LaserBeam : public GameplayObject {
public:
	LaserBeam();
	~LaserBeam();

	virtual void move(Level * level);
	virtual void process();

	virtual void hit_player(Player * player);
	virtual void hit_npc(NPC * npc);

	virtual void draw(SDL_Surface * screen, int frames_processed = 0);

	int start;

	Player * owner;
	Player * target;
};
