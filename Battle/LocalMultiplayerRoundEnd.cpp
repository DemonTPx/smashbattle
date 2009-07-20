#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include "Main.h"
#include "Player.h"
#include "Level.h"
#include "LocalMultiplayerRoundEnd.h"

#define MENU_TOP_OFFSET 368
#define MENU_ITEM_HEIGHT TILE_H
#define MENU_ITEM_WIDTH 224

#define DIRECTION_NONE	0
#define DIRECTION_LEFT	1
#define DIRECTION_RIGHT	2
#define DIRECTION_UP	4
#define DIRECTION_DOWN	8

const int LocalMultiplayerRoundEnd::ITEMCOUNT = 3;
const char * LocalMultiplayerRoundEnd::item[ITEMCOUNT] = {"CHANGE CHARACTER", "CHANGE LEVEL", "QUIT TO MENU"};

LocalMultiplayerRoundEnd::LocalMultiplayerRoundEnd(int players) {
	this->players = players;
}

void LocalMultiplayerRoundEnd::run() {
	SDL_Event event;

	init();

	Main::audio->play_music(MUSIC_END);
	
	ready = false;
	frame = 0;
	
	cursor_direction = 0;
	cursor_direction_start = 0;
	cursor_enter = false;
	cursor_first = false;

	while (Main::running && !ready) {
		while(SDL_PollEvent(&event)) {
			Main::instance->handle_event(&event);
			handle_input(&event);
		}
		process_cursor();

		frame++;
		draw();

		Main::instance->flip();
	}

	cleanup();
}

void LocalMultiplayerRoundEnd::init() {
	SDL_Surface * surface;
	SDL_Rect * rect;

	controls1 = Main::instance->controls1;
	controls2 = Main::instance->controls2;
	controls3 = Main::instance->controls3;
	controls4 = Main::instance->controls4;

	selected_item = 0;

	surf_items = new std::vector<SDL_Surface*>(0);
	surf_items_clip = new std::vector<SDL_Rect*>(0);
	for(int i = 0; i < ITEMCOUNT; i++) {
		surface = TTF_RenderText_Solid(Main::graphics->font26, item[i], Main::graphics->white);
		surf_items->push_back(surface);

		rect = new SDL_Rect();
		rect->x = (WINDOW_WIDTH - MENU_ITEM_WIDTH) / 2;
		rect->y = MENU_TOP_OFFSET + (i * MENU_ITEM_HEIGHT) + 6;
		surf_items_clip->push_back(rect);
	}
}

void LocalMultiplayerRoundEnd::cleanup() {
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
}

void LocalMultiplayerRoundEnd::draw() {
	int i;
	SDL_Surface * text;
	SDL_Rect rect, rect_s;
	SDL_Surface * screen;
	char str[30];
	double accuracy;

	screen = Main::instance->screen;

	SDL_FillRect(screen, NULL, 0);
	/*
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
	}*/

	for(i = 0; i < ITEMCOUNT; i++) {
		/*
		rect.x = surf_items_clip->at(i)->x - (TILE_W * 2);
		rect.y = surf_items_clip->at(i)->y - 6;
		SDL_BlitSurface(Main::graphics->tiles, &rect_s, screen, &rect);
		rect.x = surf_items_clip->at(i)->x + MENU_ITEM_WIDTH + TILE_W;
		SDL_BlitSurface(Main::graphics->tiles, &rect_s, screen, &rect);
		*/

		text = surf_items->at(i);
		
		if(selected_item == i) {
			rect.x = surf_items_clip->at(i)->x - TILE_W;
			rect.y = surf_items_clip->at(i)->y - 6;
			rect.w = MENU_ITEM_WIDTH + (TILE_W * 2);
			rect.h = MENU_ITEM_HEIGHT;

			SDL_FillRect(screen, &rect, 0x0088ff);
		}

		SDL_BlitSurface(text, NULL, screen, surf_items_clip->at(i));
	}
	/*
	rect.x = ((WINDOW_WIDTH - MENU_ITEM_WIDTH) / 2) - (TILE_W * 2);
	rect.y = MENU_TOP_OFFSET + (ITEMCOUNT * MENU_ITEM_HEIGHT);
	for(i = 0; i < (MENU_ITEM_WIDTH / TILE_W) + 4; i++) {
		SDL_BlitSurface(Main::graphics->tiles, &rect_s, screen, &rect);
		rect.x += TILE_W;
	}
	*/

	SDL_Rect r_block;
	
	// RESULTS
	if(winner == -1) {
		text = TTF_RenderText_Solid(Main::graphics->font52, (char*)"DRAW", Main::graphics->white);
	} else {
		sprintf_s(str, 30, "%s WINS\0", player[winner]->name);
		text = TTF_RenderText_Solid(Main::graphics->font52, str, Main::graphics->white);
	}
	rect.x = 10;
	rect.y = 10;
	if(!(frame & 0x10))
		SDL_BlitSurface(text, NULL, screen, &rect);

	// Round number
	sprintf_s(str, 20, "ROUND %d\0", round);
	text = TTF_RenderText_Solid(Main::graphics->font52, str, Main::graphics->white);
	rect.x = WINDOW_WIDTH - 10 - text->w;
	rect.y = 10;
	SDL_BlitSurface(text, NULL, screen, &rect);

	r_block.w = WINDOW_WIDTH - 20;
	r_block.h = 72;

	for(int i = 0; i < players; i++) {
		r_block.x = 10;
		r_block.y = 50 + (76 * i);

		SDL_FillRect(screen, &r_block, Player::COLORS[i]);
		
		// Avatar
		rect.x = r_block.x + 14;
		rect.y = r_block.y + 14;
		if(i == winner && (frame & 0x10))
			SDL_BlitSurface(player[i]->sprites, Main::graphics->player_clip[SPR_AVATAR_SELECTED], screen, &rect);
		else
			SDL_BlitSurface(player[i]->sprites, Main::graphics->player_clip[SPR_AVATAR], screen, &rect);

		// Player number
		rect.x = r_block.x + 72;
		rect.y = r_block.y + 10;
		sprintf_s(str, 5, "P%d\0", (i + 1));
		text = TTF_RenderText_Solid(Main::graphics->font52, str, Main::graphics->white);
		SDL_BlitSurface(text, NULL, screen, &rect);

		// Player name
		rect.x = r_block.x + 72;
		rect.y = r_block.y + 48;
		text = TTF_RenderText_Solid(Main::graphics->font26, player[i]->name, Main::graphics->white);
		SDL_BlitSurface(text, NULL, screen, &rect);

		// Bullets fired
		rect.x = r_block.x + 202;
		rect.y = r_block.y + 20;
		rect_s.x = 0;
		rect_s.y = 0;
		rect_s.w = 8;
		rect_s.h = 8;
		SDL_BlitSurface(Main::graphics->weapons, &rect_s, screen, &rect);

		sprintf_s(str, 20, "%06d\0", player[i]->bullets_fired);
		text = TTF_RenderText_Solid(Main::graphics->font26, str, Main::graphics->white);
		rect.x = r_block.x + 310 - text->w;
		rect.y = r_block.y + 16;
		SDL_BlitSurface(text, NULL, screen, &rect);
		
		// Bullets accuracy
		rect.x = r_block.x + 332;
		rect.y = r_block.y + 16;
		rect_s.x = 0;
		rect_s.y = 16;
		rect_s.w = 16;
		rect_s.h = 16;
		SDL_BlitSurface(Main::graphics->common, &rect_s, screen, &rect);

		if(player[i]->bullets_fired == 0)
			accuracy = 0;
		else
			accuracy = ((double)player[i]->bullets_hit / (double)player[i]->bullets_fired) * 100;
		sprintf_s(str, 20, "%3.0f\0", accuracy);
		text = TTF_RenderText_Solid(Main::graphics->font26, str, Main::graphics->white);
		rect.x = r_block.x + 390 - text->w;
		rect.y = r_block.y + 16;
		SDL_BlitSurface(text, NULL, screen, &rect);

		// Bombs fired
		rect.x = r_block.x + 200;
		rect.y = r_block.y + 40;
		rect_s.x = 0;
		rect_s.y = 0;
		rect_s.w = 12;
		rect_s.h = 16;
		SDL_BlitSurface(Main::graphics->bombs, &rect_s, screen, &rect);

		sprintf_s(str, 20, "%06d\0", player[i]->bombs_fired);
		text = TTF_RenderText_Solid(Main::graphics->font26, str, Main::graphics->white);
		rect.x = r_block.x + 310 - text->w;
		rect.y = r_block.y + 40;
		SDL_BlitSurface(text, NULL, screen, &rect);
		
		// Bombs accuracy
		rect.x = r_block.x + 332;
		rect.y = r_block.y + 40;
		rect_s.x = 0;
		rect_s.y = 16;
		rect_s.w = 16;
		rect_s.h = 16;
		SDL_BlitSurface(Main::graphics->common, &rect_s, screen, &rect);

		if(player[i]->bombs_fired == 0)
			accuracy = 0;
		else
			accuracy = ((double)player[i]->bombs_hit / (double)player[i]->bombs_fired) * 100;
		sprintf_s(str, 20, "%3.0f\0", accuracy);
		text = TTF_RenderText_Solid(Main::graphics->font26, str, Main::graphics->white);
		rect.x = r_block.x + 390 - text->w;
		rect.y = r_block.y + 40;
		SDL_BlitSurface(text, NULL, screen, &rect);
		
		// Rounds draw
		text = TTF_RenderText_Solid(Main::graphics->font26, (char*)"DRAW", Main::graphics->white);
		rect.x = r_block.x + r_block.w - 136;
		rect.y = r_block.y + 6;
		SDL_BlitSurface(text, NULL, screen, &rect);

		sprintf_s(str, 4, "%02d\0", player[i]->rounds_draw);
		text = TTF_RenderText_Solid(Main::graphics->font52, str, Main::graphics->white);
		rect.x = r_block.x + r_block.w - 130;
		rect.y = r_block.y + 32;
		SDL_BlitSurface(text, NULL, screen, &rect);

		// Rounds won
		text = TTF_RenderText_Solid(Main::graphics->font26, (char*)"WON", Main::graphics->white);
		rect.x = r_block.x + r_block.w - 58;
		rect.y = r_block.y + 6;
		SDL_BlitSurface(text, NULL, screen, &rect);

		sprintf_s(str, 4, "%02d\0", player[i]->rounds_won);
		text = TTF_RenderText_Solid(Main::graphics->font52, str, Main::graphics->white);
		rect.x = r_block.x + r_block.w - 60;
		rect.y = r_block.y + 32;
		SDL_BlitSurface(text, NULL, screen, &rect);
	}
}

void LocalMultiplayerRoundEnd::handle_input(SDL_Event * event) {
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

void LocalMultiplayerRoundEnd::process_cursor() {
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

void LocalMultiplayerRoundEnd::select() {
	Main::audio->play(SND_SELECT);
	switch(selected_item) {
		case 0:
			result = ROUNDEND_CHANGE_CHARACTER;
			ready = true;
			break;
		case 1:
			result = ROUNDEND_CHANGE_LEVEL;
			ready = true;
			break;
		case 2:
			result = ROUNDEND_QUIT;
			ready = true;
			break;
	}
	Main::audio->play_music(MUSIC_TITLE);
}

void LocalMultiplayerRoundEnd::select_up() {
	Main::audio->play(SND_SELECT);

	selected_item--;

	if(selected_item < 0) {
		selected_item = ITEMCOUNT - 1;
	}
}

void LocalMultiplayerRoundEnd::select_down() {
	Main::audio->play(SND_SELECT);

	selected_item++;

	if(selected_item == ITEMCOUNT) {
		selected_item = 0;
	}
}
