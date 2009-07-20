#include "SDL/SDL.h"

#include <vector>

#include "Main.h"
#include "Timer.h"
#include "AudioController.h"
#include "CharacterSelect.h"
#include "LevelSelect.h"
#include "Gameplay.h"
#include "LocalMultiplayer.h"
#include "LocalMultiplayerRoundEnd.h"
#include "Options.h"
#include "PlayerAnimation.h"

#include "Menu.h"

#define MENU_TOP_OFFSET 160
#define MENU_ITEM_HEIGHT TILE_H
#define MENU_ITEM_WIDTH 128

#define DIRECTION_NONE	0
#define DIRECTION_LEFT	1
#define DIRECTION_RIGHT	2
#define DIRECTION_UP	4
#define DIRECTION_DOWN	8

const int Menu::ITEMCOUNT = 5;
const char * Menu::item[ITEMCOUNT] = {"2P DUEL", "3P BATTLE", "4P BATTLE", "OPTIONS", "QUIT"};

Menu::Menu() {
}

Menu::~Menu() {
}

void Menu::run() {
	SDL_Event event;

	init();

	controls1 = Main::instance->controls1;
	controls2 = Main::instance->controls2;
	controls3 = Main::instance->controls3;
	controls4 = Main::instance->controls4;
	
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

		process_playeranimation();
		playeranimation->move();

		draw();

		Main::instance->flip();
		frame++;
	}
	Main::audio->stop_music();

	cleanup();
}

void Menu::draw() {
	int i;
	SDL_Surface * text;
	SDL_Rect rect, rect_s;
	SDL_Surface * screen;

	screen = Main::instance->screen;

	SDL_BlitSurface(Main::graphics->bg_menu, NULL, screen, NULL);

	rect.x = (WINDOW_WIDTH - title->w) / 2;
	rect.y = 40;
	SDL_BlitSurface(title, NULL, screen, &rect);

	rect_s.x = 0;
	rect_s.y = 0;
	rect_s.w = TILE_W;
	rect_s.h = TILE_H;

	rect.x = ((WINDOW_WIDTH - MENU_ITEM_WIDTH) / 2) - (TILE_W * 2);
	rect.y = MENU_TOP_OFFSET - TILE_H;
	for(i = 0; i < (MENU_ITEM_WIDTH / TILE_W) + 4; i++) {
		SDL_BlitSurface(Main::graphics->tiles, &rect_s, screen, &rect);
		rect.x += TILE_W;
	}

	for(i = 0; i < ITEMCOUNT; i++) {
		rect.x = surf_items_clip->at(i)->x - (TILE_W * 2);
		rect.y = surf_items_clip->at(i)->y - 8;
		SDL_BlitSurface(Main::graphics->tiles, &rect_s, screen, &rect);
		rect.x = surf_items_clip->at(i)->x + MENU_ITEM_WIDTH + TILE_W;
		SDL_BlitSurface(Main::graphics->tiles, &rect_s, screen, &rect);

		text = surf_items->at(i);
		
		if(selected_item == i) {
			rect.x = surf_items_clip->at(i)->x - TILE_W;
			rect.y = surf_items_clip->at(i)->y - 8;
			rect.w = MENU_ITEM_WIDTH + (TILE_W * 2);
			rect.h = MENU_ITEM_HEIGHT;

			SDL_FillRect(screen, &rect, 0x0088ff);
		}

		SDL_BlitSurface(text, NULL, screen, surf_items_clip->at(i));
	}

	rect.x = ((WINDOW_WIDTH - MENU_ITEM_WIDTH) / 2) - (TILE_W * 2);
	rect.y = MENU_TOP_OFFSET + (ITEMCOUNT * MENU_ITEM_HEIGHT);
	for(i = 0; i < (MENU_ITEM_WIDTH / TILE_W) + 4; i++) {
		SDL_BlitSurface(Main::graphics->tiles, &rect_s, screen, &rect);
		rect.x += TILE_W;
	}

	playeranimation->draw(Main::instance->screen);

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
			start_local_multiplayer(2);
			break;
		case 1:
			start_local_multiplayer(3);
			break;
		case 2:
			start_local_multiplayer(4);
			break;
		case 3:
			Options * options;
			options = new Options();
			options->run();
			delete options;
			controls1 = Main::instance->controls1;
			controls2 = Main::instance->controls2;
			break;
		case 4:
			SDL_Delay(500);
			Main::running = false;
			break;
	}
	Main::audio->play_music(MUSIC_TITLE);
}

void Menu::start_local_multiplayer(int players) {
	Player ** player;

	Level * level;
	LocalMultiplayer * lmp;
	LocalMultiplayerRoundEnd * end;
	CharacterSelect * cs;
	LevelSelect * ls;

	int round;

	int winner;
	int highest_score;

	bool running;
	bool change_character;
	bool change_level;

	player = new Player*[players];

	level = NULL;
	for(int i = 0; i < players; i++) {
		player[i] = new Player(0, (i + 1));
		if(i == 0) player[i]->controls = controls1;
		if(i == 1) player[i]->controls = controls2;
		if(i == 2) player[i]->controls = controls3;
		if(i == 3) player[i]->controls = controls4;
	}

	running = true;
	round = 0;

	cs = new CharacterSelect(players);
	ls = new LevelSelect(players);

	change_level = true;
	change_character = true;

	while(Main::running && running) {
		round++;

		if(change_character) {
			cs->run();
			if(cs->cancel) break;

			for(int i = 0; i < players; i++) {
				player[i]->set_character(cs->player_select[i]);
			}
		}

		for(int i = 0; i < players; i++) {
			player[i]->reset();
		}

		if(change_level) {
			ls->run();
			if(ls->cancel) break;

			if(level != NULL)
				delete level;

			level = new Level();
			level->load(Level::LEVELS[ls->level].filename);
		}

		lmp = new LocalMultiplayer();
		lmp->set_level(level);

		for(int i = 0; i < players; i++) {
			lmp->add_player(player[i]);
		}
		lmp->run();

		highest_score = 0;
		winner = -1;
		for(int i = 0; i < players; i++) {
			if(player[i]->score == highest_score) {
				winner = -1;
			}
			if(player[i]->score > highest_score) {
				winner = i;
				highest_score = player[i]->score;
			}
		}

		if(winner == -1) {
			for(int i = 0; i < players; i++) {
				if(player[i]->score == highest_score) {
					player[i]->rounds_draw++;
				}
			}
		} else {
			player[winner]->rounds_won++;
		}

		end = new LocalMultiplayerRoundEnd(players);
		end->player = player;
		end->winner = winner;
		end->round = round;
		end->run();
		if(end->result == ROUNDEND_CHANGE_CHARACTER) {
			change_level = true;
			change_character = true;
		}
		if(end->result == ROUNDEND_CHANGE_LEVEL) {
			change_character = false;
			change_level = true;
		}
		if(end->result == ROUNDEND_QUIT)
			running = false;
		delete end;

		delete lmp;
	}

	delete cs;
	delete ls;
	if(level != NULL)
		delete level;
	for(int i = 0; i < players; i++) {
		if(player[i] != NULL)
			delete player[i];
	}
	delete player;
}

void Menu::process_playeranimation() {
	if(playeranimation->position->x < -PLAYER_W)
		next_playeranimation();
	if(frame - animation_start == 0) {
		playeranimation->position->x = WINDOW_WIDTH;
		playeranimation->position->y = (TILE_H * 13) - PLAYER_H;
		playeranimation->is_walking = true;
		playeranimation->is_running = true;
		playeranimation->momentumx = -20;
		playeranimation->animate_in_place = false;
		playeranimation->direction = -1;
	}
	if(frame - animation_start == 32) {
		playeranimation->direction = 1;
	}
	if(playeranimation->momentumx == 0) {
		playeranimation->is_walking = false;
		playeranimation->direction = -1;
	}
	if(playeranimation->character == 2 && frame - animation_start == 100) {
		playeranimation->is_duck = true;
	}
	if(frame - animation_start == 140) {
		playeranimation->is_duck = false;
		playeranimation->is_walking = true;
		playeranimation->direction = -1;
	}
}

void Menu::next_playeranimation() {
	int character;
	character = playeranimation->character;
	delete playeranimation;

	playeranimation = new PlayerAnimation((character + 1) % Player::CHARACTER_COUNT);
	animation_start = frame;
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

	title = Main::text->render_text_large("SMASH BATTLE");

	surf_items = new std::vector<SDL_Surface*>(0);
	surf_items_clip = new std::vector<SDL_Rect*>(0);
	for(int i = 0; i < ITEMCOUNT; i++) {
		surface = Main::text->render_text_medium(item[i]);
		surf_items->push_back(surface);

		rect = new SDL_Rect();
		rect->x = (WINDOW_WIDTH - MENU_ITEM_WIDTH) / 2;
		rect->y = MENU_TOP_OFFSET + (i * MENU_ITEM_HEIGHT) + 8;
		surf_items_clip->push_back(rect);
	}

	credits = new std::vector<SDL_Surface*>(0);
	surface = Main::text->render_text_small("PROGRAMMING BY BERT HEKMAN");
	credits->push_back(surface);
	surface = Main::text->render_text_small("GRAPHICS BY JEROEN GROENEWEG AND OKKE VOERMAN");
	credits->push_back(surface);
	surface = Main::text->render_text_small("MUSIC BY NICK PERRIN");
	credits->push_back(surface);

	playeranimation = new PlayerAnimation(0);
	animation_start = 0;
}

void Menu::cleanup() {
	SDL_FreeSurface(title);

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

	delete playeranimation;
}
