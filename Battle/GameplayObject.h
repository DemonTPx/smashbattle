#pragma once

#include "Level.h"
#include "Player.h"
#include "NPC.h"

class CommandGeneratePowerup;

class GameplayObject {
public:
	GameplayObject();
	virtual ~GameplayObject();
	
	virtual void move(Level * level) = 0;
	virtual void process() = 0;

	virtual void hit_player(Player * player) = 0;
	virtual void hit_npc(NPC * npc) = 0;

	virtual void draw(SDL_Surface * screen, int frames_processed) = 0;

	virtual void copyTo(CommandGeneratePowerup &powerup);

	bool done;
	bool is_powerup;

	SDL_Rect * position;

	void set_id(short id) { id_ = id; };
	short id() { return id_; }

private:

	short id_;
};
