#pragma once

#include <vector>
#include "Main.h"
#include "PlayerAnimation.h"

class Menu {
public:
	Menu(Main &main);
	~Menu();
	void run();
	void draw();

	void select_up();
	void select_down();
	void select();
private:
	SDL_Surface * title;
    SDL_Surface * subtitle;

	int selected_item;

	int frame;

	GameInput * input[4];
	GameInput * input_master;
	bool started;

	std::vector<SDL_Surface*> * credits;
	std::vector<SDL_Surface*> * surf_items;
	std::vector<SDL_Rect*> * surf_items_clip;

	PlayerAnimation * playeranimation;
	int animation_start;

	bool draw_playername;
	SDL_Rect pos_playername;

	static const char * item[];
	static const int ITEMCOUNT;

	void handle_input(SDL_Event * event);
	void process_cursor();

	void start_missions();
	void start_local_multiplayer();
	void start_server_listing();

	void next_playeranimation();
	void process_playeranimation();

	void init();
	void cleanup();

	Main &main_;
};
