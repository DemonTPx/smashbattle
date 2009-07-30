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
		/*
		case 0:
			ControlsOptions * controlsoptions1;
			controlsoptions1 = new ControlsOptions(&Main::instance->controls1);
			controlsoptions1->run();
			delete controlsoptions1;
			break;
		case 1:
			ControlsOptions * controlsoptions2;
			controlsoptions2 = new ControlsOptions(&Main::instance->controls2);
			controlsoptions2->run();
			delete controlsoptions2;
			break;
		case 2:
			ControlsOptions * controlsoptions3;
			controlsoptions3 = new ControlsOptions(&Main::instance->controls3);
			controlsoptions3->run();
			delete controlsoptions3;
			break;
		case 3:
			ControlsOptions * controlsoptions4;
			controlsoptions4 = new ControlsOptions(&Main::instance->controls4);
			controlsoptions4->run();
			delete controlsoptions4;
			break;
			*/
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
