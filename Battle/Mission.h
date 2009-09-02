#ifndef _MISSION_H
#define _MISSION_H

#include "Gameplay.h"

class Mission : public Gameplay {
public:
protected:
	virtual void initialize();

	virtual void on_game_reset();

	virtual void on_pre_processing();
	virtual void on_post_processing();
};

#endif
