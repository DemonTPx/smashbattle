#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"

#include "Options.h"

Options::Options() {
	OptionItem * item;

	item = new OptionItem();
	item->name = "Controls Player 1";
	item->options = NULL;
	item->selected = 0;
	add_item(item);

	item = new OptionItem();
	item->name = "Controls Player 2";
	item->options = NULL;
	item->selected = 0;
	add_item(item);

	item = new OptionItem();
	item->name = "Sound and music";
	item->options = NULL;
	item->selected = 0;
	add_item(item);

	item = new OptionItem();
	item->name = "Return";
	item->options = NULL;
	item->selected = 0;
	add_item(item);

	OptionsScreen::align = CENTER;
}

void Options::run() {
	OptionsScreen::run();
}

void Options::exit_options() {
	OptionsScreen::running = false;
}

void Options::item_selected() {
	switch(selected_item) {
		case 3:
			running = false;
			break;
	}
}
