#ifndef _GAMEPLAY_H
#define _GAMEPLAY_H

#include <vector>

#include "Level.h"
#include "Player.h"
#include "GameplayObject.h"

class Gameplay {
public:
	Gameplay();
	~Gameplay();

	static int frame;

	void run();

	void set_level(Level * level);
	void add_player(Player * player);
protected:
	virtual void initialize();
	virtual void deinitialize();

	void reset_game();
	
	void draw_pause_screen();
	void draw_score();
	void draw_win_screen();
	void draw_countdown();

	virtual void on_pre_processing();
	virtual void on_post_processing();

	virtual void process_player_collission();
	virtual void process_npc_collission();

	void handle_pause_input(SDL_Event * event);
	
	bool game_running;

	bool paused;
	bool countdown;
	bool ended;

	SDL_Surface * screen;
	Level * level;

	std::vector<Player*> * players;
	//std::vector<NPC*> * npcs;
	
	std::vector<GameplayObject*> * objects;

};

#endif
