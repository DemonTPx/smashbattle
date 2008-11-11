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

	void init();
	void cleanup();

	void add_item(OptionItem *);

	virtual void item_selected();
	virtual void selection_changed();
private:	
	ControlScheme controls1, controls2;

	std::vector<SDL_Surface *> * surf_items;
	std::vector<SDL_Rect *> * surf_items_clip;

	TTF_Font * font26;
	TTF_Font * font13;
	SDL_Color fontColor;

	int frame;

	int menu_item_height;
	int menu_top_offset;
	int menu_left_offset;
	int menu_options_left_offset;
	int screen_w, screen_h;

	void draw();

	int cursor_direction;
	bool cursor_first;
	int cursor_direction_start;
	bool cursor_enter;

	void handle_input(SDL_Event * event);
	void process_cursor();
	
	void select_up();
	void select_down();
	void select_left();
	void select_right();
	void select();
};

#endif
