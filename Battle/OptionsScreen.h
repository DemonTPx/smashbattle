#ifndef _OPTIONSSCREEN_H
#define _OPTIONSSCREEN_H

#include <vector>
#include "Main.h"

struct OptionItem {
	char * name;
	std::vector<char *> * options;
	unsigned int selected;

	SDL_Surface * surf_name;
	SDL_Rect * rect_name;
	std::vector<SDL_Surface *> * surf_options;
	std::vector<SDL_Rect *> * rect_options;
};

class OptionsScreen {
public:
	OptionsScreen();

	void run();
protected:
	int selected_item;
	bool running;

	std::vector<OptionItem *> * items;

	enum {
		LEFT,
		CENTER,
		RIGHT
	} align;
	
	int menu_item_height;
	int menu_top_offset;
	int menu_left_offset;
	int menu_options_left_offset;

	void init();
	void cleanup();

	void add_item(OptionItem *);

	virtual void item_selected();
	virtual void selection_changed();
private:	
	GameInput * input;

	std::vector<SDL_Surface *> * surf_items;
	std::vector<SDL_Rect *> * surf_items_clip;

	SDL_Surface * background;

	int frame;

	int screen_w, screen_h;

	void draw();

	void process_cursor();
	
	void select_up();
	void select_down();
	void select_left();
	void select_right();
	void select();
};

#endif
