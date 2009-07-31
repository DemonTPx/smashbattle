#include "SDL/SDL.h"

#include <iostream>

#include "Main.h"

#include "GameInput.h"

// The value where we make the difference between a press and a non-press
const int GameInput::JOYSTICK_AXIS_THRESHOLD = 0x3fff;

// Input handling for keyboard and joystick
GameInput::GameInput() {
	joystick = NULL;

	reset();

	delay = 0;
	interval = 0;

	keyboard_enabled = true;
	joystick_enabled = false;

	keybinds = new std::vector<GameInputKeyBind>();
	joybuttonbinds = new std::vector<GameInputJoyButtonBind>();
	joyaxisbinds = new std::vector<GameInputJoyAxisBind>();
	joyhatbinds = new std::vector<GameInputJoyHatBind>();
}

GameInput::~GameInput() {
	if(joystick != NULL)
		SDL_JoystickClose(joystick);

	delete keybinds;
	delete joybuttonbinds;
	delete joyaxisbinds;
	delete joyhatbinds;
}

GameInput * GameInput::clone(bool clone_binds) {
	GameInput * gi = new GameInput();
	
	gi->enable_keyboard(keyboard_enabled);
	gi->enable_joystick(joystick_enabled);
	gi->open_joystick(get_joystick_idx());

	if(clone_binds) {
		for(unsigned int i = 0; i < keybinds->size(); i++) {
			gi->bind_key(keybinds->at(i).key, keybinds->at(i).action);
		}

		for(unsigned int i = 0; i < joybuttonbinds->size(); i++) {
			gi->bind_joybutton(joybuttonbinds->at(i).button, joybuttonbinds->at(i).action);
		}

		for(unsigned int i = 0; i < joyaxisbinds->size(); i++) {
			gi->bind_joyaxis(joyaxisbinds->at(i).axis, joyaxisbinds->at(i).threshold, joyaxisbinds->at(i).action);
		}

		for(unsigned int i = 0; i < joyhatbinds->size(); i++) {
			gi->bind_joyhat(joyhatbinds->at(i).hat, joyhatbinds->at(i).direction, joyhatbinds->at(i).action);
		}
	}

	return gi;
}

void GameInput::copy_from(GameInput * gi) {
	enable_keyboard(gi->keyboard_enabled);
	enable_joystick(gi->joystick_enabled);
	open_joystick(gi->get_joystick_idx());

	flush_keybinds();
	flush_joybuttons();
	flush_joyaxes();
	flush_joyhats();

	for(unsigned int i = 0; i < gi->keybinds->size(); i++) {
		bind_key(gi->keybinds->at(i).key, gi->keybinds->at(i).action);
	}

	for(unsigned int i = 0; i < gi->joybuttonbinds->size(); i++) {
		bind_joybutton(gi->joybuttonbinds->at(i).button, gi->joybuttonbinds->at(i).action);
	}

	for(unsigned int i = 0; i < gi->joyaxisbinds->size(); i++) {
		bind_joyaxis(gi->joyaxisbinds->at(i).axis, gi->joyaxisbinds->at(i).threshold, gi->joyaxisbinds->at(i).action);
	}

	for(unsigned int i = 0; i < gi->joyhatbinds->size(); i++) {
		bind_joyhat(gi->joyhatbinds->at(i).hat, gi->joyhatbinds->at(i).direction, gi->joyhatbinds->at(i).action);
	}
}

// Enable keyboard
void GameInput::enable_keyboard(bool enable) {
	keyboard_enabled = enable;
}

// Enable joystick
void GameInput::enable_joystick(bool enable) {
	joystick_enabled = enable;
}

// Open a joystick
bool GameInput::open_joystick(int index) {
	if(joystick != NULL) {
		SDL_JoystickClose(joystick);
	}

	joystick = SDL_JoystickOpen(index);

	if(joystick == NULL)
		return false;

	joystick_idx = index;
	joystick_enabled = true;

	return true;
}

int GameInput::get_joystick_idx() {
	return joystick_idx;
}

int GameInput::num_buttons() {
	return SDL_JoystickNumButtons(joystick);
}

int GameInput::num_axes() {
	return SDL_JoystickNumAxes(joystick);
}

int GameInput::num_hats() {
	return SDL_JoystickNumHats(joystick);
}

// Bind a keyboard key to an action
void GameInput::bind_key(int key, int action) {
	GameInputKeyBind bind;
	bind.key = key;
	bind.action = action;
	keybinds->push_back(bind);
}

// Bind a joystick button to an action
void GameInput::bind_joybutton(int button, int action) {
	GameInputJoyButtonBind bind;
	bind.button = button;
	bind.action = action;
	joybuttonbinds->push_back(bind);
}

// Bind an joystick axis button to an action
void GameInput::bind_joyaxis(int axis, bool positive, int action) {
	int threshold;
	threshold = positive ? JOYSTICK_AXIS_THRESHOLD : -JOYSTICK_AXIS_THRESHOLD;
	bind_joyaxis(axis, threshold, action);
}

// Bind an joystick axis button to an action
void GameInput::bind_joyaxis(int axis, int threshold, int action) {
	GameInputJoyAxisBind bind;
	bind.axis = axis;
	bind.threshold = threshold;
	bind.action = action;
	joyaxisbinds->push_back(bind);
}

// Bind a joystick POV-hat button to an action
void GameInput::bind_joyhat(int hat, int direction, int action) {
	GameInputJoyHatBind bind;
	bind.hat = hat;
	bind.direction = direction;
	bind.action = action;
	joyhatbinds->push_back(bind);
}

// Check if a binded button for an action is pressed
bool GameInput::is_pressed(int action) {
	if(delay == 0)
		return pressed[action];

	if(!pressed[action])
		return false;

	int frames;

	frames = Main::frame - press_start[action];

	if(frames == 0)
		return true;
	
	if(frames < delay)
		return false;

	frames -= delay;
	frames = frames % interval;

	if(frames == 0)
		return true;
	else
		return false;
}

void GameInput::reset() {
	for(int i = 0; i < ACTION_COUNT; i++) {
		pressed[i] = false;
	}
}

void GameInput::set_delay(int d, int i) {
	delay = d;
	interval = i;
}

void GameInput::unset_delay() {
	delay = 0;
	interval = 0;
}

void GameInput::handle_event(SDL_Event *event) {
	unsigned int idx;

	if(keyboard_enabled) {
		// Keyboard button
		if(event->type == SDL_KEYDOWN || event->type == SDL_KEYUP) {
			GameInputKeyBind bind;

			for(idx = 0; idx < keybinds->size(); idx++) {
				bind = keybinds->at(idx);
				if(bind.key == event->key.keysym.sym) {
					if(event->type == SDL_KEYDOWN && !pressed[bind.action]) {
						pressed[bind.action] = true;
						press_start[bind.action] = Main::frame;
					}
					if(event->type == SDL_KEYUP && pressed[bind.action]) {
						pressed[bind.action] = false;
						press_start[bind.action] = 0;
					}
				}
			}
		}
	}

	if(joystick_enabled) {
		// Joystick button
		if(event->type == SDL_JOYBUTTONDOWN || event->type == SDL_JOYBUTTONUP) {
			GameInputJoyButtonBind bind;

			if(event->jbutton.which != joystick_idx)
				return;

			for(idx = 0; idx < joybuttonbinds->size(); idx++) {
				bind = joybuttonbinds->at(idx);
				if(bind.button == event->jbutton.button) {
					if(event->type == SDL_JOYBUTTONDOWN && !pressed[bind.action]) {
						pressed[bind.action] = true;
						press_start[bind.action] = Main::frame;
					}
					if(event->type == SDL_JOYBUTTONUP && pressed[bind.action]) {
						pressed[bind.action] = false;
						press_start[bind.action] = 0;
					}
				}
			}
		}

		// Joystick axis
		if(event->type == SDL_JOYAXISMOTION) {
			GameInputJoyAxisBind bind;

			if(event->jaxis.which != joystick_idx)
				return;

			for(idx = 0; idx < joyaxisbinds->size(); idx++) {
				bind = joyaxisbinds->at(idx);
				if(bind.axis == event->jaxis.axis) {
					if(!pressed[bind.action]) {
						if(bind.threshold < 0 && bind.threshold > event->jaxis.value) {
							pressed[bind.action] = true;
							press_start[bind.action] = Main::frame;
						}
						else if(bind.threshold >= 0 && bind.threshold < event->jaxis.value) {
							pressed[bind.action] = true;
							press_start[bind.action] = Main::frame;
						}
					}
					else {
						if(bind.threshold < 0 && bind.threshold < event->jaxis.value) {
							pressed[bind.action] = false;
							press_start[bind.action] = 0;
						}
						if(bind.threshold >= 0 && bind.threshold > event->jaxis.value) {
							pressed[bind.action] = false;
							press_start[bind.action] = 0;
						}
					}
				}
			}
		}

		// Joystick hat
		if(event->type == SDL_JOYHATMOTION) {
			GameInputJoyHatBind bind;

			if(event->jhat.which != joystick_idx)
				return;

			for(idx = 0; idx < joyhatbinds->size(); idx++) {
				bind = joyhatbinds->at(idx);

				if(bind.hat == event->jhat.hat) {
					if((event->jhat.value & bind.direction) == bind.direction && !pressed[bind.action]) {
						pressed[bind.action] = true;
						press_start[bind.action] = Main::frame;
					}
					if((event->jhat.value & bind.direction) == 0 && pressed[bind.action]) {
						pressed[bind.action] = false;
						press_start[bind.action] = 0;
					}
				}
			}
		}
	}
}

// Get the current position of a joystick axis
int GameInput::get_joyaxis(int axis) {
	if(joystick_enabled || joystick != NULL) {
		SDL_JoystickUpdate();
		return SDL_JoystickGetAxis(joystick, axis);
	}
	else {
		return 0;
	}
}

// Wait until a key is pressed
int GameInput::keyboard_wait_event() {
	SDL_Event event;
	int retval;
	
	retval = 0;
	while(Main::instance->running) {
		SDL_WaitEvent(&event);
		
		Main::instance->handle_event(&event);

		if(event.type == SDL_KEYDOWN) {
			retval = event.key.keysym.sym;
			break;
		}
	}

	return retval;
}

// Wait until a button has been pressed on the joystick
void GameInput::joystick_wait_event(GameInputJoystickEvent * event) {
	SDL_Event sdlevent;
	int old_state;
	int jindex;

	event->type = event->NONE;
	event->button = 0;
	event->axis_idx = 0;
	event->axis_value = 0;
	event->hat_idx = 0;
	event->hat_direction = 0;

	if(!joystick_enabled || joystick == NULL)
		return;

	joystick_wait_released();

	old_state = SDL_JoystickEventState(SDL_QUERY);
	SDL_JoystickEventState(SDL_ENABLE);

	jindex = SDL_JoystickIndex(joystick);

	while(Main::instance->running) {
		SDL_WaitEvent(&sdlevent);
		
		Main::instance->handle_event(&sdlevent);

		if(sdlevent.type == SDL_JOYBUTTONDOWN && sdlevent.jbutton.which == jindex) {
			event->type = event->BUTTON;
			event->button = sdlevent.jbutton.button;
			break;
		}
		if(sdlevent.type == SDL_JOYAXISMOTION && sdlevent.jaxis.which == jindex) {
			if(sdlevent.jaxis.value < -JOYSTICK_AXIS_THRESHOLD || sdlevent.jaxis.value > JOYSTICK_AXIS_THRESHOLD) {
				event->type = event->AXIS;
				event->axis_idx = sdlevent.jaxis.axis;
				if(sdlevent.jaxis.value < -JOYSTICK_AXIS_THRESHOLD) {
					event->axis_value = -JOYSTICK_AXIS_THRESHOLD;
				} else {
					event->axis_value = JOYSTICK_AXIS_THRESHOLD;
				}
				break;
			}
		}
		if(sdlevent.type == SDL_JOYHATMOTION && sdlevent.jhat.which == jindex) {
			event->type = event->HAT;
			event->hat_idx = sdlevent.jhat.hat;
			event->hat_direction = sdlevent.jhat.value;
			break;
		}
	}

	SDL_JoystickEventState(old_state);
}

// Wait until all the joystick buttons are released and all the axes and hats are centered
void GameInput::joystick_wait_released() {
	int buttons, axes, hats;
	int i;

	if(!joystick_enabled || joystick == NULL)
		return;
	
	bool released;

	buttons = SDL_JoystickNumButtons(joystick);
	axes = SDL_JoystickNumAxes(joystick);
	hats = SDL_JoystickNumHats(joystick);
	
	do {
		released = true;

		SDL_JoystickUpdate();

		for(i = 0; i < buttons; i++) {
			if(SDL_JoystickGetButton(joystick, i) == 1)
				released = false;
		}

		for(i = 0; i < axes; i++) {
			if(SDL_JoystickGetAxis(joystick, i) < -JOYSTICK_AXIS_THRESHOLD ||
				SDL_JoystickGetAxis(joystick, i) > JOYSTICK_AXIS_THRESHOLD)
				released = false;
		}

		for(i = 0; i < hats; i++) {
			if(SDL_JoystickGetHat(joystick, i) != SDL_HAT_CENTERED)
				released = false;
		}
	} while(!released);
}

// Poll for a key and bind it
void GameInput::keyboard_wait_event_bind(int action) {
	int key;
	key = keyboard_wait_event();
	if(key != 0) {
		bind_key(key, action);
	}
}

// Poll for a joystick button, axis or hat press and bind it
void GameInput::joystick_wait_event_bind(int action) {
	GameInputJoystickEvent event;

	joystick_wait_event(&event);

	if(event.type == event.BUTTON) {
		bind_joybutton(event.button, action);
	}
	if(event.type == event.AXIS) {
		bind_joyaxis(event.axis_idx, event.axis_value, action);
	}
	if(event.type == event.HAT) {
		bind_joyhat(event.hat_idx, event.hat_direction, action);
	}
}

void GameInput::load_options(std::istream * stream) {
	int i;
	short count;

	count = 0;

	stream->read((char *)&joystick_idx, sizeof(int));
	open_joystick(joystick_idx);

	stream->read((char*)&count, sizeof(short));
	for(i = 0; i < count; i++) {
		GameInputKeyBind bind;
		stream->read((char *)&bind, sizeof(GameInputKeyBind));
		keybinds->push_back(bind);
	}

	stream->read((char*)&count, sizeof(short));
	for(i = 0; i < count; i++) {
		GameInputJoyButtonBind bind;
		stream->read((char *)&bind, sizeof(GameInputJoyButtonBind));
		joybuttonbinds->push_back(bind);
	}

	stream->read((char*)&count, sizeof(short));
	for(i = 0; i < count; i++) {
		GameInputJoyAxisBind bind;
		stream->read((char *)&bind, sizeof(GameInputJoyAxisBind));
		joyaxisbinds->push_back(bind);
	}

	stream->read((char*)&count, sizeof(short));
	for(i = 0; i < count; i++) {
		GameInputJoyHatBind bind;
		stream->read((char *)&bind, sizeof(GameInputJoyHatBind));
		joyhatbinds->push_back(bind);
	}
}

void GameInput::save_options(std::ostream * stream) {
	short count;

	stream->write((char *)&joystick_idx, sizeof(int));

	count = (short)keybinds->size();
	stream->write((char *)&count, sizeof(short));
	for(unsigned int i = 0; i < keybinds->size(); i++) {
		stream->write((char *)&keybinds->at(i), sizeof(GameInputKeyBind));
	}

	count = (short)joybuttonbinds->size();
	stream->write((char *)&count, sizeof(short));
	for(unsigned int i = 0; i < joybuttonbinds->size(); i++) {
		stream->write((char *)&joybuttonbinds->at(i), sizeof(GameInputJoyButtonBind));
	}

	count = (short)joyaxisbinds->size();
	stream->write((char *)&count, sizeof(short));
	for(unsigned int i = 0; i < joyaxisbinds->size(); i++) {
		stream->write((char *)&joyaxisbinds->at(i), sizeof(GameInputJoyAxisBind));
	}

	count = (short)joyhatbinds->size();
	stream->write((char *)&count, sizeof(short));
	for(unsigned int i = 0; i < joyhatbinds->size(); i++) {
		stream->write((char *)&joyhatbinds->at(i), sizeof(GameInputJoyHatBind));
	}
}

void GameInput::flush_keybinds() {
	keybinds->clear();
}

void GameInput::flush_joybuttons() {
	joybuttonbinds->clear();
}

void GameInput::flush_joyaxes() {
	joyaxisbinds->clear();
}

void GameInput::flush_joyhats() {
	joyhatbinds->clear();
}
