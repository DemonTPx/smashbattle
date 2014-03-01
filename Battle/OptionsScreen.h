#pragma once

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
	OptionsScreen(std::string title);
	virtual ~OptionsScreen() {}

	void run();
protected:
	GameInput * input;

	int selected_item;
	bool running;

	std::string title;
	std::vector<OptionItem *> * items;

	enum {
		LEFT,
		CENTER,
		RIGHT
	} align;

	int title_left_offset;
	int title_top_offset;
	
	int menu_item_height;
	int menu_top_offset;
	int menu_left_offset;
	int menu_options_left_offset;

	void init();
	void cleanup();

	void update();

	void add_item(OptionItem *);

	virtual void item_selected();
	virtual void selection_changed();
	
	virtual bool process_event(SDL_Event &event);
	virtual void process_cursor();

private:
	SDL_Surface * surf_title;
	std::vector<SDL_Surface *> * surf_items;
	std::vector<SDL_Rect *> * surf_items_clip;

	SDL_Surface * background;
	int frame;

	int screen_w, screen_h;

	void draw();
	
	virtual void on_pre_draw() {};
	virtual void on_post_draw() {};
	
	void select_up();
	void select_down();
	void select_left();
	void select_right();
	void select();

	void initialize_items();
	void cleanup_items(bool with_delete);
};
