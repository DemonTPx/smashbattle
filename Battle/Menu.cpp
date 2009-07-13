#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"

#include <vector>

#include "Main.h"
#include "Timer.h"
#include "AudioController.h"
#include "Main.h"
//#include "Battle.h"
#include "Gameplay.h"
#include "LocalMultiplayer.h"
#include "Options.h"

#include "Menu.h"

#define MENU_TOP_OFFSET 200
#define MENU_ITEM_HEIGHT 26

#define DIRECTION_NONE	0
#define DIRECTION_LEFT	1
#define DIRECTION_RIGHT	2
#define DIRECTION_UP	4
#define DIRECTION_DOWN	8

const int Menu::ITEMCOUNT = 3;
const char * Menu::item[ITEMCOUNT] = {"START", "OPTIONS", "QUIT"};

Menu::Menu() {
}

Menu::~Menu() {
}

void Menu::run() {
	SDL_Event event;

	init();

	controls1 = Main::instance->controls1;
	controls2 = Main::instance->controls2;
	
	Main::audio->play_music(MUSIC_TITLE);

	frame = 0;

	cursor_direction = 0;
	cursor_direction_start = 0;
	cursor_enter = false;
	cursor_first = false;

	while (Main::running) {
		while(SDL_PollEvent(&event)) {
			Main::instance->handle_event(&event);
			handle_input(&event);
		}
		process_cursor();
		
		draw();

		Main::instance->flip();
		frame++;
	}
	Main::audio->stop_music();

	cleanup();
}

void Menu::draw() {
	int i;
	SDL_Surface * text, * highlight;
	SDL_Rect rect;
	SDL_Surface * screen;

	screen = Main::instance->screen;

	SDL_BlitSurface(bg, NULL, screen, NULL);

	for(i = 0; i < ITEMCOUNT; i++) {
		text = surf_items->at(i);
		
		if(selected_item == i) {
			highlight = SDL_CreateRGBSurface(NULL, text->w + 10, MENU_ITEM_HEIGHT, 32, 0, 0, 0, 0);
			SDL_FillRect(highlight, NULL, 0x444488);
			
			rect.x = surf_items_clip->at(i)->x - 5;
			rect.y = surf_items_clip->at(i)->y - 3;

			SDL_BlitSurface(highlight, NULL, screen, &rect);
			SDL_FreeSurface(highlight);
		}

		SDL_BlitSurface(text, NULL, screen, surf_items_clip->at(i));
	}

	rect.x = (WINDOW_WIDTH - credits->at(0)->w) / 2;
	rect.y = WINDOW_HEIGHT - 35;
	SDL_BlitSurface(credits->at(0), NULL, screen, &rect);
	
	rect.x = (WINDOW_WIDTH - credits->at(1)->w) / 2;
	rect.y = WINDOW_HEIGHT - 25;
	SDL_BlitSurface(credits->at(1), NULL, screen, &rect);

	rect.x = (WINDOW_WIDTH - credits->at(2)->w) / 2;
	rect.y = WINDOW_HEIGHT - 15;
	SDL_BlitSurface(credits->at(2), NULL, screen, &rect);
}

void Menu::handle_input(SDL_Event * event) {
	int old_direction;
	old_direction = cursor_direction;

	if(event->type == SDL_KEYDOWN) {
		// Keyboard 1
		if(controls1.use_keyboard) {
			if(event->key.keysym.sym == controls1.kb_left)
				cursor_direction |= DIRECTION_LEFT;
			if(event->key.keysym.sym == controls1.kb_right)
				cursor_direction |= DIRECTION_RIGHT;
			if(event->key.keysym.sym == controls1.kb_up)
				cursor_direction |= DIRECTION_UP;
			if(event->key.keysym.sym == controls1.kb_down)
				cursor_direction |= DIRECTION_DOWN;
			else if(event->key.keysym.sym == controls1.kb_shoot || 
				event->key.keysym.sym == controls1.kb_run ||
				event->key.keysym.sym == controls1.kb_start ||
				(controls1.kb_up != controls1.kb_jump &&
				event->key.keysym.sym == controls1.kb_jump)) {
					cursor_enter = true;
			}
		}
		// Keyboard 2
		if(controls2.use_keyboard) {
			if(event->key.keysym.sym == controls2.kb_left)
				cursor_direction |= DIRECTION_LEFT;
			if(event->key.keysym.sym == controls2.kb_right)
				cursor_direction |= DIRECTION_RIGHT;
			if(event->key.keysym.sym == controls2.kb_up)
				cursor_direction |= DIRECTION_UP;
			if(event->key.keysym.sym == controls2.kb_down)
				cursor_direction |= DIRECTION_DOWN;
			else if(event->key.keysym.sym == controls2.kb_shoot || 
				event->key.keysym.sym == controls2.kb_run ||
				event->key.keysym.sym == controls2.kb_start ||
				(controls2.kb_up != controls2.kb_jump &&
				event->key.keysym.sym == controls2.kb_jump)) {
					cursor_enter = true;
			}
		}
	}
	if(event->type == SDL_KEYUP) {
		// Keyboard 1
		if(controls1.use_keyboard) {
			if(event->key.keysym.sym == controls1.kb_left && cursor_direction & DIRECTION_LEFT)
				cursor_direction ^= DIRECTION_LEFT;
			if(event->key.keysym.sym == controls1.kb_right && cursor_direction & DIRECTION_RIGHT)
				cursor_direction ^= DIRECTION_RIGHT;
			if(event->key.keysym.sym == controls1.kb_up && cursor_direction & DIRECTION_UP)
				cursor_direction ^= DIRECTION_UP;
			if(event->key.keysym.sym == controls1.kb_down && cursor_direction & DIRECTION_DOWN)
				cursor_direction ^= DIRECTION_DOWN;
		}
		// Keyboard 2
		if(controls2.use_keyboard) {
			if(event->key.keysym.sym == controls2.kb_left && cursor_direction & DIRECTION_LEFT)
				cursor_direction ^= DIRECTION_LEFT;
			if(event->key.keysym.sym == controls2.kb_right && cursor_direction & DIRECTION_RIGHT)
				cursor_direction ^= DIRECTION_RIGHT;
			if(event->key.keysym.sym == controls2.kb_up && cursor_direction & DIRECTION_UP)
				cursor_direction ^= DIRECTION_UP;
			if(event->key.keysym.sym == controls2.kb_down && cursor_direction & DIRECTION_DOWN)
				cursor_direction ^= DIRECTION_DOWN;
		}
	}
	if(event->type == SDL_JOYBUTTONDOWN) {
		// Joystick 1 Buttons
		if(controls1.use_joystick && event->jbutton.which == controls1.joystick_idx) {
			if(event->jbutton.button == controls1.js_left)
				cursor_direction |= DIRECTION_LEFT;
			if(event->jbutton.button == controls1.js_right)
				cursor_direction |= DIRECTION_RIGHT;
			if(event->jbutton.button == controls1.js_jump ||
				event->jbutton.button == controls1.js_run ||
				event->jbutton.button == controls1.js_shoot ||
				event->jbutton.button == controls1.js_start) {
					cursor_enter = true;
			}
		}
		// Joystick 2 Buttons
		if(controls2.use_joystick && event->jbutton.which == controls2.joystick_idx) {
			if(event->jbutton.button == controls2.js_left)
				cursor_direction |= DIRECTION_LEFT;
			if(event->jbutton.button == controls2.js_right)
				cursor_direction |= DIRECTION_RIGHT;
			if(event->jbutton.button == controls2.js_jump ||
				event->jbutton.button == controls2.js_run ||
				event->jbutton.button == controls2.js_shoot ||
				event->jbutton.button == controls2.js_start) {
					cursor_enter = true;
			}
		}
	}
	if(event->type == SDL_JOYBUTTONUP) {
		// Joystick 1 Buttons
		if(controls1.use_joystick && event->jbutton.which == controls1.joystick_idx) {
			if(event->jbutton.button == controls1.js_left && cursor_direction & DIRECTION_LEFT)
				cursor_direction ^= DIRECTION_LEFT;
			if(event->jbutton.button == controls1.js_right && cursor_direction & DIRECTION_RIGHT)
				cursor_direction ^= DIRECTION_RIGHT;
		}
		// Joystick 2 Buttons
		if(controls2.use_joystick && event->jbutton.which == controls2.joystick_idx) {
			if(event->jbutton.button == controls2.js_left && cursor_direction & DIRECTION_LEFT)
				cursor_direction ^= DIRECTION_LEFT;
			if(event->jbutton.button == controls2.js_right && cursor_direction & DIRECTION_RIGHT)
				cursor_direction ^= DIRECTION_RIGHT;
		}
	}
	if(event->type == SDL_JOYAXISMOTION) {
		// Joystick 1 Axis
		if(controls1.use_joystick && event->jbutton.which == controls1.joystick_idx) {
			if(event->jaxis.axis == 0) {
				if(event->jaxis.value < -Main::JOYSTICK_AXIS_THRESHOLD)
					cursor_direction |= DIRECTION_LEFT;
				else if(event->jaxis.value > Main::JOYSTICK_AXIS_THRESHOLD)
					cursor_direction |= DIRECTION_RIGHT;
				else {
					if(cursor_direction & DIRECTION_LEFT)
						cursor_direction ^= DIRECTION_LEFT;
					if(cursor_direction & DIRECTION_RIGHT)
						cursor_direction ^= DIRECTION_RIGHT;
				}
			}
			if(event->jaxis.axis == 1) {
				if(event->jaxis.value < -Main::JOYSTICK_AXIS_THRESHOLD)
					cursor_direction |= DIRECTION_UP;
				else if(event->jaxis.value > Main::JOYSTICK_AXIS_THRESHOLD)
					cursor_direction |= DIRECTION_DOWN;
				else {
					if(cursor_direction & DIRECTION_UP)
						cursor_direction ^= DIRECTION_UP;
					if(cursor_direction & DIRECTION_DOWN)
						cursor_direction ^= DIRECTION_DOWN;
				}
			}
		}
		// Joystick 2 Axis
		if(controls2.use_joystick && event->jbutton.which == controls2.joystick_idx) {
			if(event->jaxis.axis == 0) {
				if(event->jaxis.value < -Main::JOYSTICK_AXIS_THRESHOLD)
					cursor_direction |= DIRECTION_LEFT;
				else if(event->jaxis.value > Main::JOYSTICK_AXIS_THRESHOLD)
					cursor_direction |= DIRECTION_RIGHT;
				else {
					if(cursor_direction & DIRECTION_LEFT)
						cursor_direction ^= DIRECTION_LEFT;
					if(cursor_direction & DIRECTION_RIGHT)
						cursor_direction ^= DIRECTION_RIGHT;
				}
			}
			if(event->jaxis.axis == 1) {
				if(event->jaxis.value < -Main::JOYSTICK_AXIS_THRESHOLD)
					cursor_direction |= DIRECTION_UP;
				else if(event->jaxis.value > Main::JOYSTICK_AXIS_THRESHOLD)
					cursor_direction |= DIRECTION_DOWN;
				else {
					if(cursor_direction & DIRECTION_UP)
						cursor_direction ^= DIRECTION_UP;
					if(cursor_direction & DIRECTION_DOWN)
						cursor_direction ^= DIRECTION_DOWN;
				}
			}
		}
	}
	if(old_direction != cursor_direction) {
		cursor_first = true;
	}
}


void Menu::process_cursor() {
	int delay;

	if(cursor_enter) {
		cursor_enter = false;
		select();
	}

	if(cursor_direction != 0) {
		if(cursor_first)
			delay = 0;
		else
			delay = Main::CONTROLS_REPEAT_SPEED;
		if(frame - cursor_direction_start > delay) {
			cursor_direction_start = frame;
			cursor_first = false;
			if(cursor_direction & DIRECTION_UP) {
				select_up();
			}
			if(cursor_direction & DIRECTION_DOWN) {
				select_down();
			}
		}
	}
}

void Menu::select() {
	Main::audio->play(SND_SELECT);
	switch(selected_item) {
		case 0:
			/*
			Battle * battle;
			battle = new Battle();
			battle->run();
			delete battle;
			*/
			{
				LocalMultiplayer mp;
				Level l;
				Player p1("Bert", 1, "gfx/bert.bmp"), p2("Jeroen", 2, "gfx/jeroen.bmp");
				//Player p3("Tedje", 3, "gfx/tedje.bmp"), p4("Rob", 4, "gfx/rob.bmp");
				p1.controls = controls1;
				p2.controls = controls2;
				//p3.controls = controls1;
				//p4.controls = controls2;
				
				l.load("stage/commongrounds.stg");

				mp.set_level(&l);
				mp.add_player(&p1);
				mp.add_player(&p2);
				//mp.add_player(&p3);
				//mp.add_player(&p4);
				mp.run();
			}

			break;
		case 1:
			Options * options;
			options = new Options();
			options->run();
			delete options;
			controls1 = Main::instance->controls1;
			controls2 = Main::instance->controls2;
			break;
		case 2:
			SDL_Delay(500);
			Main::running = false;
			break;
	}
	Main::audio->play_music(MUSIC_TITLE);
}

void Menu::select_up() {
	Main::audio->play(SND_SELECT);

	selected_item--;

	if(selected_item < 0) {
		selected_item = ITEMCOUNT - 1;
	}
}

void Menu::select_down() {
	Main::audio->play(SND_SELECT);

	selected_item++;

	if(selected_item == ITEMCOUNT) {
		selected_item = 0;
	}
}

void Menu::init() {
	SDL_Surface * surface;
	SDL_Rect * rect;

	selected_item = 0;

	surface = SDL_LoadBMP("gfx/title.bmp");
	bg = SDL_DisplayFormat(surface);
	SDL_FreeSurface(surface);

	surf_items = new std::vector<SDL_Surface*>(0);
	surf_items_clip = new std::vector<SDL_Rect*>(0);
	for(int i = 0; i < ITEMCOUNT; i++) {
		surface = TTF_RenderText_Solid(Main::graphics->font26, item[i], Main::graphics->white);
		surf_items->push_back(surface);

		rect = new SDL_Rect();
		rect->x = (WINDOW_WIDTH - surface->w) / 2;
		rect->y = MENU_TOP_OFFSET + (i * MENU_ITEM_HEIGHT);
		surf_items_clip->push_back(rect);
	}

	credits = new std::vector<SDL_Surface*>(0);
	surface = TTF_RenderText_Solid(Main::graphics->font13, "Programming by Bert Hekman", Main::graphics->white);
	credits->push_back(surface);
	surface = TTF_RenderText_Solid(Main::graphics->font13, "Graphics by Jeroen Groeneweg and Okke Voerman", Main::graphics->white);
	credits->push_back(surface);
	surface = TTF_RenderText_Solid(Main::graphics->font13, "Music by Nick Perrin", Main::graphics->white);
	credits->push_back(surface);

}

void Menu::cleanup() {
	for(unsigned int i = 0; i < surf_items->size(); i++) {
		SDL_FreeSurface(surf_items->at(i));
	}
	surf_items->clear();
	delete surf_items;

	for(unsigned int i = 0; i < surf_items_clip->size(); i++) {
		delete surf_items_clip->at(i);
	}
	surf_items_clip->clear();
	delete surf_items_clip;

	for(unsigned int i = 0; i < credits->size(); i++) {
		SDL_FreeSurface(credits->at(i));
	}
	credits->clear();
	delete credits;

	SDL_FreeSurface(bg);
}
