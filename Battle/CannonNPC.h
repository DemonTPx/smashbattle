#ifndef _CANNONNPC_H
#define _CANNONNPC_H

#include "NPC.h"

class CannonNPC : public NPC {
public:
	CannonNPC();
	~CannonNPC();

	void shoot();
protected:
	virtual void process();

	virtual void reset();

	virtual void hit_player_side(Player * p);
	virtual void hit_player_top_bottom(Player * p);

	int frame_first;
	int frame_last;
};

#endif
