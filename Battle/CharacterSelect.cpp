#include "SDL/SDL.h"

#include <vector>

#include "Main.h"
#include "Player.h"
#include "PlayerAnimation.h"
#include "CharacterSelect.h"

#ifndef WIN32
#define sprintf_s snprintf
#endif

#define CHARACTERS_PER_LINE	4
#define CHARACTER_WIDTH		44
#define CHARACTER_SPACING	4

#define DIRECTION_NONE	0
#define DIRECTION_LEFT	1
#define DIRECTION_RIGHT	2
#define DIRECTION_UP	4
#define DIRECTION_DOWN	8

CharacterSelect::CharacterSelect(int players) {
	this->players = players;
}

void CharacterSelect::run() {
	SDL_Event event;

	for(int i = 0; i < players; i++) {
		player_ready[i] = false;
	
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
	cancel = false;

	player_select[0] = 0;
	if(Player::CHARACTER_COUNT <= CHARACTERS_PER_LINE) {
		player_select[1] = 1;
		player_select[2] = Player::CHARACTER_COUNT - 2;
		player_select[3] = Player::CHARACTER_COUNT - 1;
	} else {
		player_select[1] = CHARACTERS_PER_LINE - 1;
		player_select[2] = Player::CHARACTER_COUNT - CHARACTERS_PER_LINE;
		player_select[3] = Player::CHARACTER_COUNT - 1;
	}

	for(int i = 0; i < players; i++) {
		playeranimation[i] = new PlayerAnimation(player_select[i]);
		playeranimation[i]->animate_in_place = true;
		switch(i) {
			case 0:
				playeranimation[i]->position->x = 40;
				playeranimation[i]->position->y = 28;
				playeranimation[i]->direction = 1;
				break;
			case 1:
				playeranimation[i]->position->x = 600 - PLAYER_W;
				playeranimation[i]->position->y = 28;
				playeranimation[i]->direction = -1;
				playeranimation[i]->set_sprite(SPR_L);
				break;
			case 2:
				playeranimation[i]->position->x = 40;
				playeranimation[i]->position->y = 288;
				playeranimation[i]->direction = 1;
				break;
			case 3:
				playeranimation[i]->position->x = 600 - PLAYER_W;
				playeranimation[i]->position->y = 288;
				playeranimation[i]->direction = -1;
				playeranimation[i]->set_sprite(SPR_L);
				break;
		}
	}

	frame = 0;

	while (Main::running && !ready && !cancel) {
		while(SDL_PollEvent(&event)) {
			Main::instance->handle_event(&event);
			handle_input(&event);
		}

		process_cursors();

		for(int i = 0; i < players; i++) {
			name[i] = Player::CHARACTERS[player_select[i]].name;
			file[i] = Player::CHARACTERS[player_select[i]].filename;

			playeranimation[i]->move();
		}

		frame++;

		draw();

		ready = true;
		for(int i = 0; i < players; i++) {
			if(!player_ready[i] || flicker[i]) {
				ready = false;
			}
		}

		Main::instance->flip();
	}

	for(int i = 0; i < players; i++) {
		delete playeranimation[i];
	}

	if(!ready)
		cancel = true;
}

void CharacterSelect::handle_input(SDL_Event * event) {
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

void CharacterSelect::process_cursors() {
	int delay;
	bool players_ready;
	bool can_select;

	players_ready = true;
	for(int i = 0; i < players; i++) {
		if(!player_ready[i])
			players_ready = false;
	}

	for(int i = 0; i < players; i++) {
		if(cursor_enter[i]) {
			cursor_enter[i] = false;
			if(!player_ready[i]) {
				can_select = true;
				for(int idx = 0; idx < players; idx++) {
					if(i == idx) continue;
					if(player_ready[idx] && (player_select[idx] == player_select[i])) {
						can_select = false;
					}
				}
				if(can_select) {
					player_ready[i] = true;
					flicker[i] = true;
					flicker_frame[i] = 0;
					playeranimation[i]->is_walking = true;
					Main::audio->play(SND_SELECT_CHARACTER);
				}
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
				if(!player_ready[i]) {
					select(&player_select[i], cursor_direction[i]);
					playeranimation[i]->set_character(player_select[i]);
					Main::audio->play(SND_SELECT);
				}
			}
		}
	}
}

void CharacterSelect::select(int * select, int direction) {
	if(direction & DIRECTION_LEFT) {
		if(*select % CHARACTERS_PER_LINE == 0)
			*select += CHARACTERS_PER_LINE;
		*select -= 1;
	}
	if(direction & DIRECTION_RIGHT) {
		if(*select % CHARACTERS_PER_LINE == CHARACTERS_PER_LINE - 1)
			*select -= CHARACTERS_PER_LINE;
		*select += 1;
	}
	if(direction & DIRECTION_UP) {
		*select -= CHARACTERS_PER_LINE;
	}
	if(direction & DIRECTION_DOWN) {
		*select += CHARACTERS_PER_LINE;
	}

	while(*select < Player::CHARACTER_COUNT) {
		*select += Player::CHARACTER_COUNT;
	}
	while(*select >= Player::CHARACTER_COUNT) {
		*select -= Player::CHARACTER_COUNT;
	}

	/*
	for(int i = 0; i < players; i++) {
		if(&player_select[i] == select) continue;
		if(player_select[i] == *select)
			this->select(select, direction);
	}*/
}

void CharacterSelect::draw() {
	SDL_Surface * screen;
	SDL_Surface * surface;
	SDL_Surface * statsblock[3];
	SDL_Rect rect, rect_b, rect_c, rect_s;
	SDL_Rect * clip;
	Uint32 color, color_back;
	char str[5];

	screen = Main::instance->screen;

	for(int y = 0; y < WINDOW_HEIGHT; y += Main::graphics->bg_grey->h) {
		for(int x = 0; x < WINDOW_WIDTH; x += Main::graphics->bg_grey->w) {
			rect.x = x;
			rect.y = y;
			SDL_BlitSurface(Main::graphics->bg_grey, NULL, screen, &rect);
		}
	}

	// CHARACTERS

	rect_b.x = (screen->clip_rect.w - ((Main::graphics->player_clip[SPR_AVATAR]->w + (CHARACTER_SPACING * 2)) * CHARACTERS_PER_LINE)) / 2;
	rect_b.y = (screen->clip_rect.h - ((Main::graphics->player_clip[SPR_AVATAR]->h + (CHARACTER_SPACING * 2)) * (Player::CHARACTER_COUNT / CHARACTERS_PER_LINE))) / 2;
	rect_b.w = Main::graphics->player_clip[SPR_AVATAR]->w + (CHARACTER_SPACING * 2);
	rect_b.h = Main::graphics->player_clip[SPR_AVATAR]->h + (CHARACTER_SPACING * 2);

	for(int idx = 0; idx < Player::CHARACTER_COUNT; idx++) {
		if(idx > 0 && idx % CHARACTERS_PER_LINE == 0) {
			rect_b.x = (screen->clip_rect.w - ((Main::graphics->player_clip[SPR_AVATAR]->w + (CHARACTER_SPACING * 2)) * CHARACTERS_PER_LINE)) / 2;
			rect_b.y += rect_b.h;
		}

		rect.x = rect_b.x + CHARACTER_SPACING;
		rect.y = rect_b.y + CHARACTER_SPACING;

		clip = Main::graphics->player_clip[SPR_AVATAR];

		color = 0;
		color_back = 0;

		for(int i = 0; i < players; i++) {
			if(player_select[i] == idx) {
				if(color != 0)
					color = Graphics::combine_colors(color, Player::COLORS[i]);
				else
					color = Player::COLORS[i];
				if(player_ready[i]) {
					color_back = Player::COLORS[i];
					color = Player::COLORS[i];
					if(flicker[i]) {
						if(flicker_frame[i] > 0x20)
							flicker[i] = false;
						if(flicker_frame[i] & 0x4)
							color = 0xffffff;
						flicker_frame[i]++;
					}
					clip = Main::graphics->player_clip[SPR_AVATAR_SELECTED];

					break;
				}
			}
		}

		SDL_FillRect(screen, &rect_b, color);
		rect_c.x = rect_b.x + 4;
		rect_c.y = rect_b.y + 4;
		rect_c.w = rect_b.w - 8;
		rect_c.h = rect_b.h - 8;
		SDL_FillRect(screen, &rect_c, color_back);

		SDL_BlitSurface(Main::graphics->player->at(idx), clip, screen, &rect);

		for(int i = 0; i < players; i++) {
			rect_c.x = rect_b.x;
			rect_c.y = rect_b.y;
			if(i == 1 || i == 3) rect_c.x += 26;
			if(i == 2 || i == 3) rect_c.y += 26;

			rect_s.x = 16;
			rect_s.y = 0;
			rect_s.w = 26;
			rect_s.h = 26;

			if(i == 1 || i == 3) rect_s.x += 26;
			if(i == 2 || i == 3) rect_s.y += 26;

			if(player_select[i] == idx) {
				SDL_BlitSurface(Main::graphics->common, &rect_s, screen, &rect_c);
			}
		}

		rect_b.x += Main::graphics->player_clip[SPR_AVATAR]->w + (CHARACTER_SPACING * 2);
	}

	// Player stats blocks
	statsblock[0] = SDL_CreateRGBSurface(NULL, 16, 18, 32, 0, 0, 0, 0);
	SDL_FillRect(statsblock[0], NULL, 0x880000);

	statsblock[1] = SDL_CreateRGBSurface(NULL, 16, 18, 32, 0, 0, 0, 0);
	SDL_FillRect(statsblock[1], NULL, 0x888800);

	statsblock[2] = SDL_CreateRGBSurface(NULL, 16, 18, 32, 0, 0, 0, 0);
	SDL_FillRect(statsblock[2], NULL, 0x008800);

	// PLAYERS
	SDL_Rect r_block, r_pnumber,r_playername, r_ready;
	SDL_Rect r_stats;
	SDL_Rect * clip_direction;
	int stats_w, stats_h;
	int direction;
	stats_w = 100;
	stats_h = 20;

	for(int i = 0; i < players; i++) {
		r_block.w = 190;
		r_block.h = 200;
		r_pnumber.w = 60;
		r_pnumber.h = 50;
		switch(i) {
			case 0:
				r_block.x = 10;
				r_block.y = 10;
				r_pnumber.x = r_block.x + r_block.w;
				r_pnumber.y = r_block.y;
				clip_direction = Main::graphics->player_clip[SPR_R];
				r_playername.x = 50 + PLAYER_W;
				r_playername.y = 30;
				r_ready.x = 50 + PLAYER_W;
				r_ready.y = 50;
				r_stats.x = 22;
				r_stats.y = 120;
				direction = 1;
				break;
			case 1:
				r_block.x = 440;
				r_block.y = 10;
				r_pnumber.x = r_block.x - r_pnumber.w;
				r_pnumber.y = r_block.y;
				clip_direction = Main::graphics->player_clip[SPR_L];
				r_playername.x = 590 - PLAYER_W;
				r_playername.y = 30;
				r_ready.x = 590 - PLAYER_W;
				r_ready.y = 50;
				r_stats.x = 616;
				r_stats.y = 120;
				direction = -1;
				break;
			case 2:
				r_block.x = 10;
				r_block.y = 270;
				r_pnumber.x = r_block.x + r_block.w;
				r_pnumber.y = r_block.y + r_block.h - r_pnumber.h;
				clip_direction = Main::graphics->player_clip[SPR_R];
				r_playername.x = 50 + PLAYER_W;
				r_playername.y = 290;
				r_ready.x = 50 + PLAYER_W;
				r_ready.y = 310;
				r_stats.x = 22;
				r_stats.y = 380;
				direction = 1;
				break;
			case 3:
				r_block.x = 440;
				r_block.y = 270;
				r_pnumber.x = r_block.x - r_pnumber.w;
				r_pnumber.y = r_block.y + r_block.h - r_pnumber.h;
				clip_direction = Main::graphics->player_clip[SPR_L];
				r_playername.x = 590 - PLAYER_W;
				r_playername.y = 290;
				r_ready.x = 590 - PLAYER_W;
				r_ready.y = 310;
				r_stats.x = 616;
				r_stats.y = 380;
				direction = -1;
				break;
		}
		SDL_FillRect(screen, &r_block, Player::COLORS[i]);
		r_block.x += 4; r_block.w -= 8;
		r_block.y += 4; r_block.h -= 8;
		SDL_FillRect(screen, &r_block, 0);


		// Player animation and background
		r_block.x += 10; r_block.w -= 22;
		r_block.y += 8; r_block.h -= 16;

		r_block.h = (TILE_H * 2) + 18;
		SDL_FillRect(screen, &r_block, 0x2288ff);

		rect.x = r_block.x;
		rect.y = r_block.y + TILE_H + 18;
		rect_s.x = (TILE_W * 4);
		rect_s.y = 0;
		rect_s.w = TILE_W;
		rect_s.h = TILE_H;

		int offset = ((playeranimation[i]->total_distance_walked / 2) % TILE_W);

		rect_s.w = TILE_W - offset;
		rect_s.x += offset;
		if(offset < 0) {
			rect_s.w = -offset;
			rect_s.x += TILE_W;
		}
		for(int tile_i = 0; tile_i < (r_block.w / TILE_W); tile_i++) {
			SDL_BlitSurface(Main::graphics->tiles, &rect_s, screen, &rect);
			rect.x += rect_s.w;
			rect_s.w = TILE_W;
			rect_s.x = (TILE_W * 4);
		}
		rect_s.w = offset;
		if(offset < 0) {
			rect_s.w = TILE_W + offset;
		}
		SDL_BlitSurface(Main::graphics->tiles, &rect_s, screen, &rect);

		playeranimation[i]->draw(screen);

		// Player name
		surface = Main::text->render_text_medium(name[i]);
		if(direction == -1) r_playername.x -= surface->w;
		SDL_BlitSurface(surface, NULL, screen, &r_playername);
		SDL_FreeSurface(surface);

		// Player ready
		if(player_ready[i]) {
			surface = Main::text->render_text_medium("READY");
			if(direction == -1) r_ready.x -= surface->w;
			SDL_BlitSurface(surface, NULL, screen, &r_ready);
			SDL_FreeSurface(surface);
		}

		// Player number
		SDL_FillRect(screen, &r_pnumber, Player::COLORS[i]);

		sprintf_s(str, 3, "P%1d", (i + 1));
		surface = Main::text->render_text_large(str);
		r_pnumber.x += (r_pnumber.w - surface->w) / 2;
		r_pnumber.y += 10;
		SDL_BlitSurface(surface, NULL, screen, &r_pnumber);
		SDL_FreeSurface(surface);

		// Stats
		surface = Main::text->render_text_medium("SPEED");
		rect.x = r_stats.x;
		rect.y = r_stats.y;
		if(direction == -1) rect.x -= surface->w;
		SDL_BlitSurface(surface, NULL, screen, &rect);
		SDL_FreeSurface(surface);

		for(int j = 0; j <= Player::CHARACTERS[player_select[i]].speedclass; j++) {
			rect.x = r_stats.x + ((stats_w + (j * 18)) * direction);
			rect.y = r_stats.y;
			if(direction == -1) rect.x -= 18;
			SDL_BlitSurface(statsblock[j], NULL, screen, &rect);
		}

		surface = Main::text->render_text_medium("WEIGHT");
		rect.x = r_stats.x;
		rect.y = r_stats.y + stats_h;
		if(direction == -1) rect.x -= surface->w;
		SDL_BlitSurface(surface, NULL, screen, &rect);
		SDL_FreeSurface(surface);

		for(int j = 0; j <= Player::CHARACTERS[player_select[i]].weightclass; j++) {
			rect.x = r_stats.x + ((stats_w + (j * 18)) * direction);
			rect.y = r_stats.y + stats_h;
			if(direction == -1) rect.x -= 18;
			SDL_BlitSurface(statsblock[j], NULL, screen, &rect);
		}

		surface = Main::text->render_text_medium("WEAPON");
		rect.x = r_stats.x;
		rect.y = r_stats.y + (stats_h * 2);
		if(direction == -1) rect.x -= surface->w;
		SDL_BlitSurface(surface, NULL, screen, &rect);
		SDL_FreeSurface(surface);

		for(int j = 0; j <= Player::CHARACTERS[player_select[i]].weaponclass; j++) {
			rect.x = r_stats.x + ((stats_w + (j * 18)) * direction);
			rect.y = r_stats.y + (stats_h * 2);
			if(direction == -1) rect.x -= 18;
			SDL_BlitSurface(statsblock[j], NULL, screen, &rect);
		}

		surface = Main::text->render_text_medium("BOMB");
		rect.x = r_stats.x;
		rect.y = r_stats.y + (stats_h * 3);
		if(direction == -1) rect.x -= surface->w;
		SDL_BlitSurface(surface, NULL, screen, &rect);
		SDL_FreeSurface(surface);

		for(int j = 0; j <= Player::CHARACTERS[player_select[i]].bombpowerclass; j++) {
			rect.x = r_stats.x + ((stats_w + (j * 18)) * direction);
			rect.y = r_stats.y + (stats_h * 3);
			if(direction == -1) rect.x -= 18;
			SDL_BlitSurface(statsblock[j], NULL, screen, &rect);
		}
	}

	SDL_FreeSurface(statsblock[0]);
	SDL_FreeSurface(statsblock[1]);
	SDL_FreeSurface(statsblock[2]);
}
