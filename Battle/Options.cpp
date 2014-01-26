#include "SDL/SDL.h"

#include "AudioOptions.h"
#include "ControlsOptions.h"

#include "Options.h"

Options::Options() : OptionsScreen("OPTIONS") {
	OptionItem * item;

	item = new OptionItem();
	item->name = (char*)"CONTROLS PLAYER 1";
	item->options = NULL;
	item->selected = 0;
	add_item(item);

	item = new OptionItem();
	item->name = (char*)"CONTROLS PLAYER 2";
	item->options = NULL;
	item->selected = 0;
	add_item(item);

	item = new OptionItem();
	item->name = (char*)"CONTROLS PLAYER 3";
	item->options = NULL;
	item->selected = 0;
	add_item(item);

	item = new OptionItem();
	item->name = (char*)"CONTROLS PLAYER 4";
	item->options = NULL;
	item->selected = 0;
	add_item(item);

	item = new OptionItem();
	item->name = (char*)"SOUND AND MUSIC";
	item->options = NULL;
	item->selected = 0;
	add_item(item);

	item = new OptionItem();
	item->name = (char*)"RETURN";
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
		case 1:
		case 2:
		case 3:
		{
			ControlsOptions co(Main::instance->input[selected_item], selected_item + 1);
			co.run();
			break;
		}
		case 4:
		{
			AudioOptions audiooptions;
			audiooptions.run();
			break;
		}
		case 5:
			running = false;
			break;
	}
}
