#include "SDL/SDL.h"

#include "ControlsOptions.h"

ControlsOptions::ControlsOptions(/*ControlScheme * controls*/) {
	OptionItem * item;

	//this->controls = controls;

	keyboard_redefined = false;
	joystick_redefined = false;
	joystick_idx_changed = false;

	item = new OptionItem();
	item->name = (char*)"USE KEYBOARD";
	item->options = new std::vector<char *>(0);
	item->options->push_back((char*)"YES");
	item->options->push_back((char*)"NO");
	//item->selected = controls->use_keyboard ? 0 : 1;
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
	//item->selected = controls->use_joystick ? 0 : 1;
	add_item(item);

	item = new OptionItem();
	item->name = (char*)"SELECT JOYSTICK";
	item->options = NULL;
	item->selected = 0;
	add_item(item);

	item = new OptionItem();
	item->name = (char*)"USE X AXIS FOR MOVEMENT";
	item->options = new std::vector<char *>(0);
	item->options->push_back((char*)"YES");
	item->options->push_back((char*)"NO");
	//item->selected = controls->use_axis_x ? 0 : 1;
	add_item(item);

	item = new OptionItem();
	item->name = (char*)"USE Y AXIS FOR JUMP";
	item->options = new std::vector<char *>(0);
	item->options->push_back((char*)"YES");
	item->options->push_back((char*)"NO");
	//item->selected = controls->use_axis_up ? 0 : 1;
	add_item(item);

	item = new OptionItem();
	item->name = (char*)"USE Y AXIS FOR DUCK";
	item->options = new std::vector<char *>(0);
	item->options->push_back((char*)"YES");
	item->options->push_back((char*)"NO");
	//item->selected = controls->use_axis_down ? 0 : 1;
	add_item(item);

	item = new OptionItem();
	item->name = (char*)"REDEFINE JOYSTICK CONTROLS";
	item->options = NULL;
	item->selected = 0;
	add_item(item);

	item = new OptionItem();
	item->name = (char*)"RETURN AND SAVE";
	item->options = NULL;
	item->selected = 0;
	add_item(item);

	item = new OptionItem();
	item->name = (char*)"RETURN AND CANCEL";
	item->options = NULL;
	item->selected = 0;
	add_item(item);

	menu_options_left_offset = 400;

	OptionsScreen::align = LEFT;
}

void ControlsOptions::run() {
	OptionsScreen::run();
}

void ControlsOptions::item_selected() {
	switch(selected_item) {
		case 1: // Redefine keyboard controls
			redefine_keyboard();
			break;
		case 3: // Select joystick
			/*
			JoystickSelect * joystickselect;
			joystickselect = new JoystickSelect(controls->joystick_idx);
			joystickselect->run();
			if(joystickselect->index > -1) {
				joystick_idx_changed = true;
				new_controls.joystick_idx = joystickselect->index;
			}
			delete joystickselect;
			*/
			break;
		case 7: // Redefine joystick controls
			redefine_joystick();
			break;
		case 8: // Return save
			if(items->at(0)->selected == 1 && items->at(2)->selected == 1) {
				show_notification("ENABLE KEYBOARD OR JOYSTICK");
				SDL_Delay(1000);
				return;
			}
			/*
			controls->use_keyboard = items->at(0)->selected == 0 ? true : false;
			if(keyboard_redefined) {
				controls->kb_left = new_controls.kb_left;
				controls->kb_right = new_controls.kb_right;
				controls->kb_up = new_controls.kb_up;
				controls->kb_down = new_controls.kb_down;
				controls->kb_run = new_controls.kb_run;
				controls->kb_jump = new_controls.kb_jump;
				controls->kb_shoot = new_controls.kb_shoot;
				controls->kb_bomb = new_controls.kb_bomb;
				controls->kb_start = new_controls.kb_start;
			}
			controls->use_joystick = items->at(2)->selected == 0 ? true : false;
			if(joystick_idx_changed)
				controls->joystick_idx = new_controls.joystick_idx;
			if(joystick_redefined) {
				controls->js_left = new_controls.js_left;
				controls->js_right = new_controls.js_right;
				controls->js_down = new_controls.js_down;
				controls->js_run = new_controls.js_run;
				controls->js_jump = new_controls.js_jump;
				controls->js_shoot = new_controls.js_shoot;
				controls->js_bomb = new_controls.js_bomb;
				controls->js_start = new_controls.js_start;
			}
			controls->use_axis_x = items->at(4)->selected == 0 ? true : false;
			controls->use_axis_up = items->at(5)->selected == 0 ? true : false;
			controls->use_axis_down = items->at(6)->selected == 0 ? true : false;
			*/
		case 9: // Return cancel
			running = false;
			break;
	}
}

void ControlsOptions::redefine_keyboard() {
	/*
	new_controls.kb_left = poll_keyboard("PRESS LEFT");
	new_controls.kb_right = poll_keyboard("PRESS RIGHT");
	new_controls.kb_up = poll_keyboard("PRESS UP");
	new_controls.kb_down = poll_keyboard("PRESS DOWN");
	new_controls.kb_run = poll_keyboard("PRESS RUN");
	new_controls.kb_jump = poll_keyboard("PRESS JUMP");
	new_controls.kb_shoot = poll_keyboard("PRESS SHOOT");
	new_controls.kb_bomb = poll_keyboard("PRESS BOMB");
	new_controls.kb_start = poll_keyboard("PRESS START");

	keyboard_redefined = true;
	*/
}

int ControlsOptions::poll_keyboard(const char * question) {
	SDL_Event event;
	bool has_key = false;
	int key = 0;

	show_notification(question);

	while(Main::running && !has_key) {
		while(SDL_PollEvent(&event)) {
			Main::instance->handle_event(&event);

			if(event.type == SDL_KEYDOWN) {
				key = event.key.keysym.sym;
			}
			if(event.type == SDL_KEYUP) {
				if(event.key.keysym.sym == key) {
					has_key = true;
					break;
				}
			}
		}
	}
	
	return key;
}

void ControlsOptions::redefine_joystick() {
	/*
	int idx;
	idx = joystick_idx_changed ? new_controls.joystick_idx : controls->joystick_idx;

	if(items->at(4)->selected == 1) { // Only poll for left and right buttons, if we do not use the axis for this
		new_controls.js_left = poll_joystick(idx, "PRESS LEFT");
		new_controls.js_right = poll_joystick(idx, "PRESS RIGHT");
	}
	new_controls.js_run = poll_joystick(idx, "PRESS RUN");
	if(items->at(5)->selected == 1) { // Only poll for jump button, if we do not use the axis for this
		new_controls.js_jump = poll_joystick(idx, "PRESS JUMP");
	}
	if(items->at(6)->selected == 1) { // Only poll for down/duck button, if we do not use the axis for this
		new_controls.js_down = poll_joystick(idx, "PRESS DOWN");
	}
	new_controls.js_shoot = poll_joystick(idx, "PRESS SHOOT");
	new_controls.js_bomb = poll_joystick(idx, "PRESS BOMB");
	new_controls.js_start = poll_joystick(idx, "PRESS START");

	joystick_redefined = true;
	*/
}

int ControlsOptions::poll_joystick(int index, const char * question) {
	SDL_Event event;
	bool has_button = false;
	int button = 0;

	show_notification(question);

	while(Main::running && !has_button) {
		while(SDL_PollEvent(&event)) {
			Main::instance->handle_event(&event);
			
			if(event.jbutton.which == index) {
				if(event.type == SDL_JOYBUTTONDOWN) {
					button = event.jbutton.button;
				}
				if(event.type == SDL_JOYBUTTONUP) {
					if(event.jbutton.button == button) {
						has_button = true;
						break;
					}
				}
			}
		}
	}
	
	return button;
}

void ControlsOptions::show_notification(const char * text) {
	SDL_Surface * screen;
	SDL_Surface * surface;
	SDL_Rect rect;

	screen = Main::instance->screen;

	surface = SDL_CreateRGBSurface(NULL, screen->w / 2, 50, 32, 0, 0, 0, 0);
	rect.x = 100;
	rect.y = screen->h / 2 - 25;
	rect.w = screen->w - 200;
	rect.h = 50;
	SDL_FillRect(screen, &rect, 0xff0000);
	rect.x += 2;
	rect.y += 2;
	rect.w -= 4;
	rect.h -= 4;
	SDL_FillRect(screen, &rect, 0);

	surface = Main::text->render_text_medium(text);
	rect.x = (screen->w - surface->w) / 2;
	rect.y = (screen->h - surface->h) / 2;
	SDL_BlitSurface(surface, NULL, screen, &rect);
	SDL_FreeSurface(surface);

	Main::instance->flip();
}

// Joystick select
JoystickSelect::JoystickSelect(int index) {
	OptionItem * item;

	this->index = index;

	for(int i = 0; i < SDL_NumJoysticks(); i++) {
		item = new OptionItem();
		item->name = (char*)SDL_JoystickName(i);
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
