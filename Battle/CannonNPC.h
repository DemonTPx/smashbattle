#pragma once

#include "NPC.h"

class CannonNPC : public NPC {
public:
	CannonNPC(Main &main);
	~CannonNPC();

	void shoot();
protected:
	virtual void process();

	virtual void reset();

	virtual void hit_player_side(Player * p);
	virtual void hit_player_top_bottom(Player * p);

	int frame_first;
	int frame_last;
	
	Main &main_;
};
