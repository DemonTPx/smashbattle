#include "SDL/SDL.h"

#include "ControlsOptions.h"
#include <sstream>
#include "Color.h"

#ifndef WIN32
#define sprintf_s snprintf
#endif

ControlsOptions::ControlsOptions(GameInput * input, int number, Main &main) : OptionsScreen(main) {
	OptionItem * item;

	std::stringstream ss;
	ss << "OPTIONS > CONTROLS PLAYER " << number;
	title = ss.str();

	this->input = input;

	new_input = input->clone();

	item = new OptionItem();
	item->name = (char*)"USE KEYBOARD";
	item->options = new std::vector<char *>(0);
	item->options->push_back((char*)"YES");
	item->options->push_back((char*)"NO");
	item->selected = input->keyboard_enabled ? 0 : 1;
	add_item(item);

	item = new OptionItem();
	item->name = (char*)"REDEFINE KEYBOARD CONTROLS";
	item->options = NULL;
	item->selected = 0;
	add_item(item);

	item = new OptionItem();
	item->name = (char*)"USE JOYSTICK";
	item->options = new std::vector<char *>(0);
	item->options->push_back((char*)"YES");
	item->options->push_back((char*)"NO");
	item->selected = input->joystick_enabled ? 0 : 1;
	add_item(item);

	item = new OptionItem();
	item->name = (char*)"SELECT JOYSTICK";
	item->options = NULL;
	item->selected = 0;
	add_item(item);

	item = new OptionItem();
	item->name = (char*)"REDEFINE JOYSTICK CONTROLS";
	item->options = NULL;
	item->selected = 0;
	add_item(item);

	menu_options_left_offset = 400;

	OptionsScreen::align = LEFT;
}

ControlsOptions::~ControlsOptions() {
	delete new_input;
}

void ControlsOptions::run() {
	keyboard_enabled = input->keyboard_enabled;
	joystick_enabled = input->joystick_enabled;

	OptionsScreen::run();

	save_input();
}

void ControlsOptions::item_selected() {
	switch(selected_item) {
		case 1: // Redefine keyboard controls
			if(items->at(0)->selected == 1) {
				show_notification("ENABLE KEYBOARD FIRST");
				SDL_Delay(1000);
				return;
			}
			new_input->flush_keybinds();
			redefine_keyboard();
			break;
		case 3: // Select joystick
			if(items->at(2)->selected == 1) {
				show_notification("ENABLE JOYSTICK FIRST");
				SDL_Delay(1000);
				return;
			}
			JoystickSelect * joystickselect;
			joystickselect = new JoystickSelect(new_input->get_joystick_idx(), main_);
			joystickselect->run();
			if(joystickselect->index > -1) {
				new_input->open_joystick(joystickselect->index);
			}
			delete joystickselect;
			break;
		case 4: // Redefine joystick controls
			if(items->at(2)->selected == 1) {
				show_notification("ENABLE JOYSTICK FIRST");
				SDL_Delay(1000);
				return;
			}
			new_input->flush_joybuttons();
			redefine_joystick();
			break;
	}

	for(int i = 0; i < 4; i++) {
		main_.input[i]->reset();
	}
}

void ControlsOptions::process_cursor() {
	if (OptionsScreen::input->is_pressed(A_BACK) && ! save_permitted()) {
		show_notification("ENABLE KEYBOARD OR JOYSTICK");
		SDL_Delay(1000);
		return;
	}

	keyboard_enabled = (items->at(0)->selected == 0);
	joystick_enabled = (items->at(2)->selected == 0);

	OptionsScreen::process_cursor();
}

void ControlsOptions::redefine_keyboard() {
	poll_keyboard(A_LEFT, "PRESS LEFT");
	poll_keyboard(A_RIGHT, "PRESS RIGHT");
	poll_keyboard(A_UP, "PRESS UP");
	poll_keyboard(A_DOWN, "PRESS DOWN");
	poll_keyboard(A_RUN, "PRESS RUN");
	poll_keyboard(A_JUMP, "PRESS JUMP");
	poll_keyboard(A_SHOOT, "PRESS SHOOT");
	poll_keyboard(A_BOMB, "PRESS BOMB");
	poll_keyboard(A_START, "PRESS START");
	poll_keyboard(A_BACK, "PRESS BACK");
}

void ControlsOptions::redefine_joystick() {
	if(new_input->num_axes() < 2 && new_input->num_hats() < 1) {
		poll_joystick(A_LEFT, "PRESS LEFT");
		poll_joystick(A_RIGHT, "PRESS RIGHT");
		poll_joystick(A_UP, "PRESS UP");
		poll_joystick(A_DOWN, "PRESS DOWN");
	}
	poll_joystick(A_RUN, "PRESS RUN");
	poll_joystick(A_JUMP, "PRESS JUMP");
	poll_joystick(A_SHOOT, "PRESS SHOOT");
	poll_joystick(A_BOMB, "PRESS BOMB");
	poll_joystick(A_START, "PRESS START");
	poll_joystick(A_BACK, "PRESS BACK");

	input->reset();
}

void ControlsOptions::poll_joystick(int action, const char * question) {
	show_notification(question);
	new_input->joystick_wait_event_bind(action);
}

void ControlsOptions::poll_keyboard(int action, const char * question) {
	show_notification(question);
	new_input->keyboard_wait_event_bind(action);
}

void ControlsOptions::show_notification(const char * text) {
	SDL_Surface * screen;
	SDL_Surface * surface;
	SDL_Rect rect;

	screen = main_.screen;

	rect.x = 100;
	rect.y = screen->h / 2 - 25;
	rect.w = screen->w - 200;
	rect.h = 50;
	SDL_FillRectColor(screen, &rect, 0xff0000);
	rect.x += 2;
	rect.y += 2;
	rect.w -= 4;
	rect.h -= 4;
	SDL_FillRectColor(screen, &rect, 0);

	surface = main_.text->render_text_medium(text);
	rect.x = (screen->w - surface->w) / 2;
	rect.y = (screen->h - surface->h) / 2;
	SDL_BlitSurface(surface, NULL, screen, &rect);
	SDL_FreeSurface(surface);

	main_.flip();
}

void ControlsOptions::save_input() {
	input->copy_from(new_input);

	input->enable_keyboard(keyboard_enabled);
	input->enable_joystick(joystick_enabled);

	if (input->joystick_enabled) {
		if (input->num_axes() >= 2) {
			input->bind_joyaxis(0, false, A_LEFT);
			input->bind_joyaxis(0, true, A_RIGHT);
			input->bind_joyaxis(1, false, A_UP);
			input->bind_joyaxis(1, true, A_DOWN);
		}
		if (input->num_axes() >= 1) {
			input->bind_joyhat(0, SDL_HAT_LEFT, A_LEFT);
			input->bind_joyhat(0, SDL_HAT_RIGHT, A_RIGHT);
			input->bind_joyhat(0, SDL_HAT_UP, A_UP);
			input->bind_joyhat(0, SDL_HAT_DOWN, A_DOWN);
		}

	}
}

bool ControlsOptions::save_permitted() {
	// Keyboard and joystick are both disabled...
	return (items->at(0)->selected == 0 || items->at(2)->selected == 0);
}

// Joystick select
JoystickSelect::JoystickSelect(int index, Main &main) : OptionsScreen(main) {
	OptionItem * item;

	this->index = index;

	for(int i = 0; i < SDL_NumJoysticks(); i++) {
		item = new OptionItem();
		sprintf_s(joystick_name[i], 80, "%s", SDL_JoystickName(i));
		item->name = joystick_name[i];
		item->options = NULL;
		add_item(item);
	}
	
	cancel_idx = (int)items->size();

	item = new OptionItem();
	item->name = (char*)"Cancel";
	item->options = NULL;
	add_item(item);

	if(index < cancel_idx) {
		selected_item = index;
	}
	
	OptionsScreen::align = LEFT;
}

void JoystickSelect::item_selected() {
	if(selected_item == cancel_idx) {
		index = -1;
	} else {
		index = selected_item;
	}
	running = false;
}
