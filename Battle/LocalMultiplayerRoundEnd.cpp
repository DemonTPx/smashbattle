#include "SDL/SDL.h"

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

#define BEGIN_DELAY 150

#ifndef WIN32
#define sprintf_s snprintf
#endif

const int LocalMultiplayerRoundEnd::ITEMCOUNT = 3;
const char * LocalMultiplayerRoundEnd::item[ITEMCOUNT] = {"CHANGE LEVEL", "CHANGE CHARACTER", "QUIT TO MENU"};

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

	// FIXME: Player statistics must be calculated before running

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
	SDL_Surface * text;
	SDL_Rect * clip;
	SDL_Rect rect, rect_s;
	SDL_Rect r_block;
	double accuracy;
	char str[30];
	short temp;
	int last_score;
	int cup;
	Player * pl;

	controls1 = Main::instance->controls1;
	controls2 = Main::instance->controls2;
	controls3 = Main::instance->controls3;
	controls4 = Main::instance->controls4;

	selected_item = 0;

	// Menu
	surf_items = new std::vector<SDL_Surface*>(0);
	surf_items_clip = new std::vector<SDL_Rect*>(0);
	for(int i = 0; i < ITEMCOUNT; i++) {
		surface = Main::text->render_text_medium(item[i]);
		surf_items->push_back(surface);

		clip = new SDL_Rect();
		clip->x = (WINDOW_WIDTH - MENU_ITEM_WIDTH) / 2;
		clip->y = MENU_TOP_OFFSET + (i * MENU_ITEM_HEIGHT) + 6;
		surf_items_clip->push_back(clip);
	}

	// Result
	if(winner == -1) {
		surf_result = Main::text->render_text_large("DRAW");
	} else {
		sprintf_s(str, 30, "%s WINS", player[winner]->name);
		surf_result = Main::text->render_text_large(str);
	}

	// Determine player positions
	for(int i = 0; i < players; i++) {
		order[i] = i;
	}
	for(short i = 0; i < players - 1; i++ ) {
		for(short j = 0; j < players - 1; j++) {
			if(player[order[j]]->score < player[order[j + 1]]->score) {
				temp = order[j];
				order[j] = order[j + 1];
				order[j + 1] = temp;
			}
		}
	}

	// Player statistics
	surf_statistics = new std::vector<SDL_Surface*>(0);

	r_block.x = 0;
	r_block.y = 0;
	r_block.w = WINDOW_WIDTH - 20;
	r_block.h = 72;

	last_score = -1;
	cup = 0;

	for(int i = 0; i < players; i++) {
		pl = player[order[i]];

		surface = SDL_CreateRGBSurface(0, r_block.w, r_block.h, 32, 0, 0, 0, 0);

		SDL_FillRect(surface, &r_block, Player::COLORS[order[i]]);

		rect.x = r_block.x + 4;
		rect.y = r_block.y + 4;
		rect.w = r_block.w - 8;
		rect.h = r_block.h - 8;
		SDL_FillRect(surface, &rect, 0);

		// Player number
		rect.x = r_block.x + 72;
		rect.y = r_block.y + 10;
		sprintf_s(str, 5, "P%d", (order[i] + 1));
		text = Main::text->render_text_large(str);
		SDL_BlitSurface(text, NULL, surface, &rect);
		SDL_FreeSurface(text);

		// Player name
		rect.x = r_block.x + 72;
		rect.y = r_block.y + 48;
		text = Main::text->render_text_medium(pl->name);
		SDL_BlitSurface(text, NULL, surface, &rect);
		SDL_FreeSurface(text);

		// Cup
		if(cup < 3) {
			if(last_score == -1 || last_score > pl->score) {
				if(last_score != -1)
					cup++;
				last_score = pl->score;
			}
			rect_s.x = (CUP_W * cup);
			rect_s.y = 0;
			rect_s.w = CUP_W;
			rect_s.h = CUP_H;
			rect.x = r_block.x + 130;
			rect.y = r_block.y + 10;
			SDL_BlitSurface(Main::graphics->cups, &rect_s, surface, &rect);
		}

		// Bullets fired
		rect.x = r_block.x + 224;
		rect.y = r_block.y + 10;
		rect_s.x = 0;
		rect_s.y = 0;
		rect_s.w = 8;
		rect_s.h = 8;
		SDL_BlitSurface(Main::graphics->weapons, &rect_s, surface, &rect);

		sprintf_s(str, 20, "%d", pl->bullets_fired);
		text = Main::text->render_text_medium_gray(str);
		rect.x = r_block.x + 240;
		rect.y = r_block.y + 8;
		SDL_BlitSurface(text, NULL, surface, &rect);
		SDL_FreeSurface(text);
		
		// Bullets accuracy
		rect.x = r_block.x + 332;
		rect.y = r_block.y + 6;
		rect_s.x = 0;
		rect_s.y = 16;
		rect_s.w = 16;
		rect_s.h = 16;
		SDL_BlitSurface(Main::graphics->common, &rect_s, surface, &rect);

		if(pl->bullets_fired == 0)
			accuracy = 0;
		else
			accuracy = ((double)pl->bullets_hit / (double)pl->bullets_fired) * 100;
		sprintf_s(str, 20, "%1.0f%%", accuracy);
		text = Main::text->render_text_medium_gray(str);
		rect.x = r_block.x + 352;
		rect.y = r_block.y + 8;
		SDL_BlitSurface(text, NULL, surface, &rect);
		SDL_FreeSurface(text);

		// Bombs fired
		rect.x = r_block.x + 222;
		rect.y = r_block.y + 28;
		rect_s.x = 12;
		rect_s.y = 0;
		rect_s.w = 12;
		rect_s.h = 16;
		SDL_BlitSurface(Main::graphics->bombs, &rect_s, surface, &rect);

		sprintf_s(str, 20, "%d", pl->bombs_fired);
		text = Main::text->render_text_medium_gray(str);
		rect.x = r_block.x + 240;
		rect.y = r_block.y + 30;
		SDL_BlitSurface(text, NULL, surface, &rect);
		SDL_FreeSurface(text);
		
		// Bombs accuracy
		rect.x = r_block.x + 332;
		rect.y = r_block.y + 28;
		rect_s.x = 0;
		rect_s.y = 16;
		rect_s.w = 16;
		rect_s.h = 16;
		SDL_BlitSurface(Main::graphics->common, &rect_s, surface, &rect);

		if(pl->bombs_fired == 0)
			accuracy = 0;
		else
			accuracy = ((double)pl->bombs_hit / (double)pl->bombs_fired) * 100;
		sprintf_s(str, 20, "%1.0f%%", accuracy);
		text = Main::text->render_text_medium_gray(str);
		rect.x = r_block.x + 352;
		rect.y = r_block.y + 30;
		SDL_BlitSurface(text, NULL, surface, &rect);
		SDL_FreeSurface(text);

		// Headstomps
		rect.x = r_block.x + 220;
		rect.y = r_block.y + 50;
		rect_s.x = 0;
		rect_s.y = 32;
		rect_s.w = 16;
		rect_s.h = 16;
		SDL_BlitSurface(Main::graphics->common, &rect_s, surface, &rect);

		sprintf_s(str, 20, "%d", pl->headstomps);
		text = Main::text->render_text_medium_gray(str);
		rect.x = r_block.x + 240;
		rect.y = r_block.y + 52;
		SDL_BlitSurface(text, NULL, surface, &rect);
		SDL_FreeSurface(text);

		// Score
		sprintf_s(str, 4, "%d", pl->score);
		text = Main::text->render_text_large(str);
		rect.x = r_block.x + r_block.w - text->w - 22;
		rect.y = r_block.y + 10;
		SDL_BlitSurface(text, NULL, surface, &rect);
		SDL_FreeSurface(text);

		// Rounds won
		sprintf_s(str, 20, "%d", pl->rounds_won);
		text = Main::text->render_text_medium_gray(str);
		rect.x = r_block.x + r_block.w - text->w - 22;
		rect.y = r_block.y + r_block.h - text->h - 10;
		SDL_BlitSurface(text, NULL, surface, &rect);
		SDL_FreeSurface(text);

		// Add surface to vector
		surf_statistics->push_back(surface);
	}
}

void LocalMultiplayerRoundEnd::cleanup() {
	for(unsigned int i = 0; i < surf_items->size(); i++) {
		SDL_FreeSurface(surf_items->at(i));
	}
	delete surf_items;

	for(unsigned int i = 0; i < surf_items_clip->size(); i++) {
		delete surf_items_clip->at(i);
	}
	delete surf_items_clip;

	SDL_FreeSurface(surf_result);

	for(unsigned int i = 0; i < surf_statistics->size(); i++) {
		SDL_FreeSurface(surf_statistics->at(i));
	}
	delete surf_statistics;
}

void LocalMultiplayerRoundEnd::draw() {
	int i;
	SDL_Surface * text;
	SDL_Rect rect;
	SDL_Surface * screen;

	screen = Main::instance->screen;

	for(int y = 0; y < WINDOW_HEIGHT; y += Main::graphics->bg_grey->h) {
		for(int x = 0; x < WINDOW_WIDTH; x += Main::graphics->bg_grey->w) {
			rect.x = x;
			rect.y = y;
			SDL_BlitSurface(Main::graphics->bg_grey, NULL, screen, &rect);
		}
	}
	
	// MENU
	if(frame >= BEGIN_DELAY) {
		for(i = 0; i < ITEMCOUNT; i++) {

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
	}

	SDL_Rect r_block;
	
	// RESULTS
	rect.x = (WINDOW_WIDTH - surf_result->w) / 2;
	rect.y = 10;
	if(!(frame & 0x20))
		SDL_BlitSurface(surf_result, NULL, screen, &rect);

	r_block.w = WINDOW_WIDTH - 20;
	r_block.h = 72;

	for(int i = 0; i < players; i++) {
		r_block.x = 10;
		r_block.y = 50 + (76 * i);

		// Pre-rendered block with statistics
		SDL_BlitSurface(surf_statistics->at(i), 0, screen, &r_block);
		
		// Avatar
		rect.x = r_block.x + 14;
		rect.y = r_block.y + 14;
		if(order[i] == winner && (frame & 0x10))
			SDL_BlitSurface(player[order[i]]->sprites, Main::graphics->player_clip[SPR_AVATAR_SELECTED], screen, &rect);
		else
			SDL_BlitSurface(player[order[i]]->sprites, Main::graphics->player_clip[SPR_AVATAR], screen, &rect);
	}
}

void LocalMultiplayerRoundEnd::handle_input(SDL_Event * event) {
	if(frame < BEGIN_DELAY)
		return;

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
			result = ROUNDEND_CHANGE_LEVEL;
			ready = true;
			break;
		case 1:
			result = ROUNDEND_CHANGE_CHARACTER;
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
