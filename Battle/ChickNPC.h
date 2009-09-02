#ifndef _CHICKNPC_H
#define _CHICKNPC_H

#include "NPC.h"

class ChickNPC : public NPC {
public:
	ChickNPC();
	~ChickNPC();
protected:
	virtual void process();
};

#endif
