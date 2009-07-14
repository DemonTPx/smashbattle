#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"

#include "Main.h"
#include "Player.h"

#include <vector>

#include "LevelSelect.h"

#ifndef WIN32
#define sprintf_s snprintf
#endif

#define LEVELS_PER_LINE	4
#define LEVEL_WIDTH		44
#define LEVEL_HEIGHT	34
#define LEVEL_SPACING	4

#define DIRECTION_NONE	0
#define DIRECTION_LEFT	1
#define DIRECTION_RIGHT	2
#define DIRECTION_UP	4
#define DIRECTION_DOWN	8

LevelSelect::LevelSelect(int players) {
	this->players = players;
}

void LevelSelect::run() {
	SDL_Event event;

	load_sprites();

	for(int i = 0; i < players; i++) {
		cursor_direction[i] = DIRECTION_NONE;
		cursor_direction_start[i] = 0;
		cursor_first[i] = true;
		cursor_enter[i] = false;
	}
	controls[0] = Main::instance->controls1;
	controls[1] = Main::instance->controls2;
	controls[2] = Main::instance->controls3;
	controls[3] = Main::instance->controls4;

	ready = false;
	ready_level = false;
	cancel = false;
	
	level = 0;
	select(DIRECTION_NONE);

	frame = 0;

	while (Main::running && !ready) {
		while(SDL_PollEvent(&event)) {
			Main::instance->handle_event(&event);
			handle_input(&event);
		}

		process_cursors();

		frame++;

		draw();

		if(ready_level && flicker_frame > 0x20) {
			ready = true;
		}

		Main::instance->flip();
	}

	if(!ready)
		cancel = true;

	Main::audio->stop_music();

	free_sprites();
}

void LevelSelect::handle_input(SDL_Event * event) {
	int old_direction[4];

	for(int i = 0; i < players; i++) {
		old_direction[i] = cursor_direction[i];

		// Keyboard
		if(event->type == SDL_KEYDOWN) {
			// Escape key always returns to main menu
			if(event->key.keysym.sym == SDLK_ESCAPE) {
				ready = true;
				cancel = true;
			}

			if(controls[i].use_keyboard) {
				if(event->key.keysym.sym == controls[i].kb_left)
					cursor_direction[i] |= DIRECTION_LEFT;
				if(event->key.keysym.sym == controls[i].kb_right)
					cursor_direction[i] |= DIRECTION_RIGHT;
				if(event->key.keysym.sym == controls[i].kb_up)
					cursor_direction[i] |= DIRECTION_UP;
				if(event->key.keysym.sym == controls[i].kb_down)
					cursor_direction[i] |= DIRECTION_DOWN;
				else if(event->key.keysym.sym == controls[i].kb_shoot || 
					event->key.keysym.sym == controls[i].kb_run ||
					(controls[i].kb_up != controls[i].kb_jump &&
					event->key.keysym.sym == controls[i].kb_jump)) {
						cursor_enter[i] = true;
				}
			}
		}
		if(event->type == SDL_KEYUP) {
			if(controls[i].use_keyboard) {
				if(event->key.keysym.sym == controls[i].kb_left && cursor_direction[i] & DIRECTION_LEFT)
					cursor_direction[i] ^= DIRECTION_LEFT;
				if(event->key.keysym.sym == controls[i].kb_right && cursor_direction[i] & DIRECTION_RIGHT)
					cursor_direction[i] ^= DIRECTION_RIGHT;
				if(event->key.keysym.sym == controls[i].kb_up && cursor_direction[i] & DIRECTION_UP)
					cursor_direction[i] ^= DIRECTION_UP;
				if(event->key.keysym.sym == controls[i].kb_down && cursor_direction[i] & DIRECTION_DOWN)
					cursor_direction[i] ^= DIRECTION_DOWN;
				else if(event->key.keysym.sym == controls[i].kb_shoot || 
					event->key.keysym.sym == controls[i].kb_run ||
					(controls[i].kb_up != controls[i].kb_jump &&
					event->key.keysym.sym == controls[i].kb_jump)) {
						cursor_enter[i] = false;
				}
			}
		}
		// Joystick Buttons
		if(event->type == SDL_JOYBUTTONDOWN) {
			if(controls[i].use_joystick && event->jbutton.which == controls[i].joystick_idx) {
				if(event->jbutton.button == controls[i].js_left)
					cursor_direction[i] |= DIRECTION_LEFT;
				if(event->jbutton.button == controls[i].js_right)
					cursor_direction[i] |= DIRECTION_RIGHT;
				if(event->jbutton.button == controls[i].js_jump ||
					event->jbutton.button == controls[i].js_run ||
					event->jbutton.button == controls[i].js_shoot) {
						cursor_enter[i] = true;
				}
			}
		}
		if(event->type == SDL_JOYBUTTONUP) {
			if(controls[i].use_joystick && event->jbutton.which == controls[i].joystick_idx) {
				if(event->jbutton.button == controls[i].js_left && cursor_direction[i] & DIRECTION_LEFT)
					cursor_direction[i] ^= DIRECTION_LEFT;
				if(event->jbutton.button == controls[i].js_right && cursor_direction[i] & DIRECTION_RIGHT)
					cursor_direction[i] ^= DIRECTION_RIGHT;
			}
		}
		// Joystick Axis
		if(event->type == SDL_JOYAXISMOTION) {
			if(controls[i].use_joystick && event->jbutton.which == controls[i].joystick_idx) {
				if(event->jaxis.axis == 0) {
					if(event->jaxis.value < -Main::JOYSTICK_AXIS_THRESHOLD)
						cursor_direction[i] |= DIRECTION_LEFT;
					else if(event->jaxis.value > Main::JOYSTICK_AXIS_THRESHOLD)
						cursor_direction[i] |= DIRECTION_RIGHT;
					else {
						if(cursor_direction[i] & DIRECTION_LEFT)
							cursor_direction[i] ^= DIRECTION_LEFT;
						if(cursor_direction[i] & DIRECTION_RIGHT)
							cursor_direction[i] ^= DIRECTION_RIGHT;
					}
				} 
				if(event->jaxis.axis == 1) {
					if(event->jaxis.value < -Main::JOYSTICK_AXIS_THRESHOLD)
						cursor_direction[i] |= DIRECTION_UP;
					else if(event->jaxis.value > Main::JOYSTICK_AXIS_THRESHOLD)
						cursor_direction[i] |= DIRECTION_DOWN;
					else {
						if(cursor_direction[i] & DIRECTION_UP)
							cursor_direction[i] ^= DIRECTION_UP;
						if(cursor_direction[i] & DIRECTION_DOWN)
							cursor_direction[i] ^= DIRECTION_DOWN;
					}
				}
			}
		}

		if(cursor_direction[i] != old_direction[i]) {
			cursor_first[i] = true;
		}
	}
}

void LevelSelect::process_cursors() {
	int delay;

	for(int i = 0; i < players; i++) {
		if(cursor_enter[i]) {
			cursor_enter[i] = false;
			Main::audio->play(SND_SELECT_CHARACTER);
			if(!ready_level) {
				ready_level = true;
				flicker = true;
				flicker_frame = 0;
			}
		}

		if(cursor_direction[i] != DIRECTION_NONE) {
			if(cursor_first[i])
				delay = 0;
			else
				delay = Main::CONTROLS_REPEAT_SPEED;
			if(frame - cursor_direction_start[i] > delay) {
				cursor_direction_start[i] = frame;
				cursor_first[i] = false;
				if(!ready_level) {
					select(cursor_direction[i]);
					Main::audio->play(SND_SELECT);
				}
			}
		}
	}
}

void LevelSelect::select(int direction) {
	if(direction & DIRECTION_LEFT) {
		if(level % LEVELS_PER_LINE == 0)
			level += LEVELS_PER_LINE;
		level--;
	}
	if(direction & DIRECTION_RIGHT) {
		if(level % LEVELS_PER_LINE == LEVELS_PER_LINE - 1)
			level -= LEVELS_PER_LINE;
		level++;
	}
	if(direction & DIRECTION_UP) {
		level -= LEVELS_PER_LINE;
	}
	if(direction & DIRECTION_DOWN) {
		level += LEVELS_PER_LINE;
	}

	if(level < 0) level += Level::LEVEL_COUNT;
	if(level >= Level::LEVEL_COUNT) level -= Level::LEVEL_COUNT;

	name = Level::LEVELS[level].name;
}

void LevelSelect::draw() {
	SDL_Surface * screen;
	SDL_Surface * surface;
	SDL_Rect rect, rect_b;
	Uint32 color;

	screen = Main::instance->screen;

	SDL_FillRect(screen, NULL, 0);

	// STAGES
	surface = TTF_RenderText_Solid(Main::graphics->font26, name, Main::graphics->white);
	rect.x = (screen->w - surface->w) / 2;
	rect.y = 200;
	SDL_BlitSurface(surface, NULL, screen, &rect);
	SDL_FreeSurface(surface);

	rect_b.x = (screen->w - ((LEVEL_WIDTH + (LEVEL_SPACING * 2)) * LEVELS_PER_LINE)) / 2;
	rect_b.y = 220;
	rect_b.w = LEVEL_WIDTH + (LEVEL_SPACING * 2);
	rect_b.h = LEVEL_HEIGHT + (LEVEL_SPACING * 2);

	for(int idx = 0; idx < Level::LEVEL_COUNT; idx++) {
		if(idx > 0 && idx % LEVELS_PER_LINE == 0) {
			rect_b.x = (screen->w - ((LEVEL_WIDTH + (LEVEL_SPACING * 2)) * LEVELS_PER_LINE)) / 2;
			rect_b.y += rect_b.h;
		}

		rect.x = rect_b.x + LEVEL_SPACING;
		rect.y = rect_b.y + LEVEL_SPACING;

		color = 0;

		if(level == idx) {
			color = 0x444488;
			
			if(ready_level && flicker) {
				if(flicker_frame > 0x20)
					flicker = false;
				if(flicker_frame & 0x4)
					color = 0xffffff;
				flicker_frame++;
			}
		}
		SDL_FillRect(screen, &rect_b, color);

		SDL_BlitSurface(thumbs->at(idx), NULL, screen, &rect);

		rect_b.x += LEVEL_WIDTH + (LEVEL_SPACING * 2);
	}
}

void LevelSelect::load_sprites() {
	SDL_Surface * surface;

	thumbs = new std::vector<SDL_Surface*>(0);

	for(int idx = 0; idx < Level::LEVEL_COUNT; idx++) {
		surface = Level::get_thumbnail(Level::LEVELS[idx].filename);
		thumbs->push_back(surface);
	}
}

void LevelSelect::free_sprites() {
	thumbs->clear();
	delete thumbs;
}
