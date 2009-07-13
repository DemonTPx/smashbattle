#ifndef _MENU_H
#define _MENU_H

#include <vector>
#include "Main.h"

class Menu {
public:
	Menu();
	~Menu();
	void run();
	void draw();

	void select_up();
	void select_down();
	void select();
private:
	SDL_Surface * bg;
	int selected_item;

	int frame;
	
	ControlScheme controls1, controls2;

	std::vector<SDL_Surface*> * credits;
	std::vector<SDL_Surface*> * surf_items;
	std::vector<SDL_Rect*> * surf_items_clip;

	static const char * item[];
	static const int ITEMCOUNT;

	int cursor_direction;
	bool cursor_first;
	int cursor_direction_start;
	bool cursor_enter;

	void handle_input(SDL_Event * event);
	void process_cursor();

	void init();
	void cleanup();
};

#endif

