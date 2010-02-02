#ifndef _MISSION_H
#define _MISSION_H

#include "Gameplay.h"

enum MISSION_TYPE {
	MT_KILL_NPCS,
	MT_GET_ITEMS,
};

struct MISSION_TARGET {
	MISSION_TYPE type;
	int time_platinum;
	int time_gold;
	int time_silver;
};

struct MISSION_INFO {
	char * name;
	char * filename;
};

class Mission : public Gameplay {
public:
	static const int MISSION_COUNT;
	static const MISSION_INFO MISSIONS[];

	Mission();

	int time;

	bool mission_ended;
	bool player_won;
	int cup;
protected:
	virtual void initialize();

	virtual void on_game_reset();

	virtual void pause(Player * p);

	virtual void on_pre_processing();
	virtual void on_post_processing();
	
	virtual void draw_score();
	virtual void draw_game_ended();
};

#endif
