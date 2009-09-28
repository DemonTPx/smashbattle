#ifndef _GAMEPLAYOBJECT_H
#define _GAMEPLAYOBJECT_H

#include "Level.h"
#include "Player.h"
#include "NPC.h"

class GameplayObject {
public:
	GameplayObject();
	virtual ~GameplayObject();
	
	virtual void move(Level * level) = 0;
	virtual void process() = 0;

	virtual void hit_player(Player * player) = 0;
	virtual void hit_npc(NPC * npc) = 0;

	virtual void draw(SDL_Surface * screen) = 0;

	bool done;
	bool is_powerup;

	SDL_Rect * position;
};

#endif
