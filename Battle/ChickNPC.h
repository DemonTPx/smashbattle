#ifndef _CHICKNPC_H
#define _CHICKNPC_H

#include "NPC.h"

class ChickNPC : public NPC {
public:
	ChickNPC();
	~ChickNPC();
protected:
	virtual void process();

	virtual void reset();

	virtual void hit_player_side(Player * p);
	virtual void hit_player_top_bottom(Player * p);
};

#endif
