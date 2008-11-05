#ifndef _MENU_H
#define _MENU_H

#include <vector>

class Menu {
public:
	Menu();
	~Menu();
	void run();
	void draw();

	void handle_input(SDL_Event * event);

	void select_up();
	void select_down();
	void select();
private:
	TTF_Font * font26;
	TTF_Font * font13;
	SDL_Color fontColor;
	SDL_Surface * bg;
	int selected_item;

	std::vector<SDL_Surface*> * credits;
	std::vector<SDL_Surface*> * surf_items;
	std::vector<SDL_Rect*> * surf_items_clip;

	static const char * item[];
	static const int ITEMCOUNT;

	void init();
	void cleanup();
};

#endif

