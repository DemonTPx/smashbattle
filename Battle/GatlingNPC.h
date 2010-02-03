#ifndef _GATLINGNPC_H
#define _GATLINGNPC_H

#include "NPC.h"

class GatlingNPC : public NPC {
public:
	GatlingNPC();
	~GatlingNPC();

	void shoot();
protected:
	virtual void process();

	virtual void reset();

	virtual void hit_player_side(Player * p);
	virtual void hit_player_top_bottom(Player * p);

	int frame_first;
	int frame_last;

	int shoot_start;

	SDL_Rect range;
};

#endif
