#ifndef _AIRSTRIKE_H
#define _AIRSTRIKE_H

#include "GameplayObject.h"

class Airstrike : public GameplayObject {
public:
	Airstrike();
	~Airstrike();

	virtual void move(Level * level);
	virtual void process();

	virtual void hit_player(Player * player);
	virtual void hit_npc(NPC * npc);

	virtual void draw(SDL_Surface * screen);

	static const int DELAY;

	int start;

	Player * owner;
};

#endif
