#include "SDL/SDL.h"

#include "AudioOptions.h"
#include "ControlsOptions.h"

#include "Options.h"

Options::Options() {
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
			ControlsOptions * co1;
			co1 = new ControlsOptions(Main::instance->input[0]);
			co1->run();
			delete co1;
			break;
		case 1:
			ControlsOptions * co2;
			co2 = new ControlsOptions(Main::instance->input[1]);
			co2->run();
			delete co2;
			break;
		case 2:
			ControlsOptions * co3;
			co3 = new ControlsOptions(Main::instance->input[2]);
			co3->run();
			delete co3;
			break;
		case 3:
			ControlsOptions * co4;
			co4 = new ControlsOptions(Main::instance->input[3]);
			co4->run();
			delete co4;
			break;
		case 4:
			AudioOptions * audiooptions;
			audiooptions = new AudioOptions();
			audiooptions->run();
			delete audiooptions;
			break;
		case 5:
			running = false;
			break;
	}
}
