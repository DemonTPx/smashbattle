#pragma once

#include "GameplayObject.h"

class LaserBeam : public GameplayObject {
public:
	LaserBeam(Main &main);
	~LaserBeam();

	virtual void move(Level * level);
	virtual void process();

	virtual void hit_player(Player * player);
	virtual void hit_npc(NPC * npc);

	int start;

	Player * owner;
	Player * target;

	Main &main_;

protected:

	virtual void draw_impl(SDL_Surface * screen, int frames_processed = 0);

};
