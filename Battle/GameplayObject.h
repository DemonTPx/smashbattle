#pragma once

#include "Level.h"
#include "Player.h"
#include "NPC.h"
#include "Drawable.h"

namespace network {
class CommandGeneratePowerup;
}

class Main;

class GameplayObject : public Drawable {
public:
	GameplayObject(Main &main);
	virtual ~GameplayObject();
	
	virtual void move(Level * level) = 0;
	virtual void process() = 0;

	virtual void hit_player(Player * player) = 0;
	virtual void hit_npc(NPC * npc) = 0;

	virtual void copyTo(network::CommandGeneratePowerup &powerup);

	bool done;
	bool is_powerup;

	SDL_Rect * position;

	void set_id(short id) { id_ = id; };
	short id() { return id_; }

private:

	short id_;

	Main &main_;
};
