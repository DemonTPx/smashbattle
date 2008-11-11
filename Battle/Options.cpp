#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"

#include "AudioOptions.h"
#include "ControlsOptions.h"

#include "Options.h"

Options::Options() {
	OptionItem * item;

	item = new OptionItem();
	item->name = (char*)"Controls Player 1";
	item->options = NULL;
	item->selected = 0;
	add_item(item);

	item = new OptionItem();
	item->name = (char*)"Controls Player 2";
	item->options = NULL;
	item->selected = 0;
	add_item(item);

	item = new OptionItem();
	item->name = (char*)"Sound and music";
	item->options = NULL;
	item->selected = 0;
	add_item(item);

	item = new OptionItem();
	item->name = (char*)"Return";
	item->options = NULL;
	item->selected = 0;
	add_item(item);

	OptionsScreen::align = LEFT;
}

void Options::run() {
	OptionsScreen::run();
}

void Options::item_selected() {
	switch(selected_item) {
		case 0:
			ControlsOptions * controlsoptions1;
			controlsoptions1 = new ControlsOptions(&Main::instance->controls1);
			controlsoptions1->run();
			delete controlsoptions1;
			reload_controls();
			break;
		case 1:
			ControlsOptions * controlsoptions2;
			controlsoptions2 = new ControlsOptions(&Main::instance->controls2);
			controlsoptions2->run();
			delete controlsoptions2;
			reload_controls();
			break;
		case 2:
			AudioOptions * audiooptions;
			audiooptions = new AudioOptions();
			audiooptions->run();
			delete audiooptions;
			break;
		case 3:
			running = false;
			break;
	}
}
