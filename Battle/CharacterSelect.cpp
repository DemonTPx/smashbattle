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

CharacterSelect::CharacterSelect() {}

void CharacterSelect::run() {
	SDL_Event event;

	ready = false;
	cancel = false;

	init();

	frame = 0;

	while (Main::running && !ready && !cancel) {
		while(SDL_PollEvent(&event)) {
			Main::instance->handle_event(&event);

			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
				cancel = true;
				break;
			}
			
			for(int i = 0; i < 4; i++) {
				input[i]->handle_event(&event);
			}
		}

		process_cursors();

		for(int i = 0; i < 4; i++) {
			if(player_joined[i]) {
				name[i] = Player::CHARACTERS[player_select[i]].name;
				file[i] = Player::CHARACTERS[player_select[i]].filename;

				playeranimation[i]->move();
			}
		}

		process_random_players();

		frame++;

		draw();

		int num_players = 0;
		ready = true;
		for(int i = 0; i < 4; i++) {
			if(player_joined[i] && player_ready[i] && !flicker[i]) {
				++num_players;
			}
			if((player_joined[i] && !player_ready[i]) || flicker[i]) {
				ready = false;
			}
		}
		if(num_players < 2) ready = false;

		Main::instance->flip();
	}

	clean_up();

	if(!ready)
		cancel = true;
}

void CharacterSelect::init() {
	// Cursor
	for(int i = 0; i < 4; i++) {
		input[i] = Main::instance->input[i];
		input[i]->set_delay();
		input[i]->reset();

		player_joined[i] = false;
		if(input[i] == Main::instance->input_master) {
			player_joined[i] = true;
		}
		player_ready[i] = false;

		player_random[i] = false;
		player_random_start[i] = 0;

		flicker[i] = false;
	}

	// Set inital cursor positions
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

	// Create player animations
	for(int i = 0; i < 4; i++) {
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

	srand(SDL_GetTicks());

	prerender_background();
}

void CharacterSelect::clean_up() {
	for(int i = 0; i < 4; i++) {
		delete playeranimation[i];
	}

	SDL_FreeSurface(background);
}

void CharacterSelect::prerender_background() {
	SDL_Surface * surface;
	SDL_Rect rect;
	char str[5];

	background = SDL_CreateRGBSurface(0, WINDOW_WIDTH, WINDOW_HEIGHT, 32, 0, 0, 0, 0);

	for(int y = 0; y < WINDOW_HEIGHT; y += Main::graphics->bg_grey->h) {
		for(int x = 0; x < WINDOW_WIDTH; x += Main::graphics->bg_grey->w) {
			rect.x = x;
			rect.y = y;
			SDL_BlitSurface(Main::graphics->bg_grey, NULL, background, &rect);
		}
	}

	// PLAYERS
	SDL_Rect r_block, r_pnumber;
	SDL_Rect r_stats;
	int stats_w, stats_h;
	int direction;
	stats_w = 100;
	stats_h = 20;

	for(int i = 0; i < 4; i++) {
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
				r_stats.x = 22;
				r_stats.y = 120;
				direction = 1;
				break;
			case 1:
				r_block.x = 440;
				r_block.y = 10;
				r_pnumber.x = r_block.x - r_pnumber.w;
				r_pnumber.y = r_block.y;
				r_stats.x = 616;
				r_stats.y = 120;
				direction = -1;
				break;
			case 2:
				r_block.x = 10;
				r_block.y = 270;
				r_pnumber.x = r_block.x + r_block.w;
				r_pnumber.y = r_block.y + r_block.h - r_pnumber.h;
				r_stats.x = 22;
				r_stats.y = 380;
				direction = 1;
				break;
			case 3:
				r_block.x = 440;
				r_block.y = 270;
				r_pnumber.x = r_block.x - r_pnumber.w;
				r_pnumber.y = r_block.y + r_block.h - r_pnumber.h;
				r_stats.x = 616;
				r_stats.y = 380;
				direction = -1;
				break;
		}
		SDL_FillRect(background, &r_block, Player::COLORS[i]);
		r_block.x += 4; r_block.w -= 8;
		r_block.y += 4; r_block.h -= 8;
		SDL_FillRect(background, &r_block, 0);

		// Player number
		SDL_FillRect(background, &r_pnumber, Player::COLORS[i]);

		sprintf_s(str, 3, "P%1d", (i + 1));
		surface = Main::text->render_text_large(str);
		r_pnumber.x += (r_pnumber.w - surface->w) / 2;
		r_pnumber.y += 10;
		SDL_BlitSurface(surface, NULL, background, &r_pnumber);
		SDL_FreeSurface(surface);

		// Stats
		surface = Main::text->render_text_medium("SPEED");
		rect.x = r_stats.x;
		rect.y = r_stats.y + 2;
		if(direction == -1) rect.x -= surface->w;
		SDL_BlitSurface(surface, NULL, background, &rect);
		SDL_FreeSurface(surface);

		surface = Main::text->render_text_medium("WEIGHT");
		rect.x = r_stats.x;
		rect.y = r_stats.y + stats_h + 2;
		if(direction == -1) rect.x -= surface->w;
		SDL_BlitSurface(surface, NULL, background, &rect);
		SDL_FreeSurface(surface);

		surface = Main::text->render_text_medium("WEAPON");
		rect.x = r_stats.x;
		rect.y = r_stats.y + (stats_h * 2) + 2;
		if(direction == -1) rect.x -= surface->w;
		SDL_BlitSurface(surface, NULL, background, &rect);
		SDL_FreeSurface(surface);

		surface = Main::text->render_text_medium("BOMB");
		rect.x = r_stats.x;
		rect.y = r_stats.y + (stats_h * 3) + 2;
		if(direction == -1) rect.x -= surface->w;
		SDL_BlitSurface(surface, NULL, background, &rect);
		SDL_FreeSurface(surface);
	}
}

void CharacterSelect::process_cursors() {
	bool players_ready;
	bool can_select;
	int direction;

	players_ready = true;
	for(int i = 0; i < 4; i++) {
		if(player_joined[i] && !player_ready[i])
			players_ready = false;
	}

	for(int i = 0; i < 4; i++) {
		if(input[i]->is_pressed(A_RUN) || input[i]->is_pressed(A_JUMP) ||
			input[i]->is_pressed(A_SHOOT) || input[i]->is_pressed(A_BOMB) ||
			input[i]->is_pressed(A_START)) {
				if(!(input[i]->is_pressed(A_JUMP) && input[i]->is_pressed(A_UP)))  { // It's likely that up and jump are the same keybind
					if(!player_joined[i]) {
						Main::audio->play(SND_SELECT);
						player_joined[i] = true;
					} else if(!player_ready[i]) {
						can_select = true;
						for(int idx = 0; idx < 4; idx++) {
							if(i == idx) continue;
							if(player_ready[idx] && (player_select[idx] == player_select[i])) {
								can_select = false;
							}
						}
						if(can_select) {
							player_ready[i] = true;

							player_random[i] = false;

							flicker[i] = true;
							flicker_frame[i] = 0;

							playeranimation[i]->is_walking = true;
							Main::audio->play(SND_SELECT_CHARACTER);
						}
					}
				}
		}

		direction = 0;
		if(input[i]->is_pressed(A_LEFT)) direction |= DIRECTION_LEFT;
		if(input[i]->is_pressed(A_RIGHT)) direction |= DIRECTION_RIGHT;
		if(input[i]->is_pressed(A_UP)) direction |= DIRECTION_UP;
		if(input[i]->is_pressed(A_DOWN)) direction |= DIRECTION_DOWN;
		if(direction != DIRECTION_NONE) {
			if(player_joined[i] && !player_ready[i]) {
				select(i, direction);
				playeranimation[i]->set_character(player_select[i]);
				Main::audio->play(SND_SELECT);
			}
		}
	}
}

void CharacterSelect::process_random_players() {
	int last;
	bool is_last;
	bool can_select;

	for(int i = 0; i < 4; i++) {
		if(player_random[i] && (frame - player_random_start[i] == 6)) {
			last = player_select[i];

			do {
				is_last = false;
				player_select[i] = rand() % Player::CHARACTER_COUNT;

				if(player_select[i] == last) is_last = true;
				
				can_select = true;
				if(!is_last) {
					for(int idx = 0; idx < 4; idx++) {
						if(i == idx) continue;
						if(player_ready[idx] && (player_select[idx] == player_select[i])) {
							can_select = false;
							break;
						}
					}
				}
			} while(!can_select || is_last);

			playeranimation[i]->set_character(player_select[i]);

			player_random_start[i] = frame;
		}
	}
}

void CharacterSelect::select(int player, int direction) {
	if(!player_random[player]) {
		// Left
		if(direction & DIRECTION_LEFT) {
			if(player_select[player] % CHARACTERS_PER_LINE == 0)
				player_select[player] += CHARACTERS_PER_LINE;
			player_select[player] -= 1;
		}

		// Right
		if(direction & DIRECTION_RIGHT) {
			if(player_select[player] % CHARACTERS_PER_LINE == CHARACTERS_PER_LINE - 1)
				player_select[player] -= CHARACTERS_PER_LINE;
			player_select[player] += 1;
		}
	}

	// Up
	if(direction & DIRECTION_UP) {
		if(player_random[player]) {
			if(player_random_before[player] >= CHARACTERS_PER_LINE)
				player_select[player] = player_random_before[player];
			else
				player_select[player] = player_random_before[player] - CHARACTERS_PER_LINE + Player::CHARACTER_COUNT;
			player_random[player] = false;
		} else if(player_select[player] < CHARACTERS_PER_LINE) {
			player_random[player] = true;
			player_random_start[player] = frame;
			player_random_before[player] = player_select[player];
		} else {
			player_select[player] -= CHARACTERS_PER_LINE;
		}
	}

	// Down
	if(direction & DIRECTION_DOWN) {
		if(player_random[player]) {
			if(player_random_before[player] < CHARACTERS_PER_LINE)
				player_select[player] = player_random_before[player];
			else
				player_select[player] = player_random_before[player] + CHARACTERS_PER_LINE - Player::CHARACTER_COUNT;
			player_random[player] = false;
		} else if(player_select[player] >= Player::CHARACTER_COUNT - CHARACTERS_PER_LINE) {
			player_random[player] = true;
			player_random_start[player] = frame;
			player_random_before[player] = player_select[player];
		} else {
			player_select[player] += CHARACTERS_PER_LINE;
		}
	}
}

void CharacterSelect::draw() {
	SDL_Surface * screen;
	SDL_Surface * surface;
	SDL_Rect rect, rect_b, rect_c, rect_s;
	SDL_Rect * clip;
	Uint32 color, color_back;

	screen = Main::instance->screen;

	SDL_BlitSurface(background, 0, screen, 0);

	// CHARACTERS: Random
	rect_b.x = (screen->clip_rect.w - ((Main::graphics->player_clip[SPR_AVATAR]->w + (CHARACTER_SPACING * 2)) * CHARACTERS_PER_LINE)) / 2;
	rect_b.y = (screen->clip_rect.h - ((Main::graphics->player_clip[SPR_AVATAR]->h + (CHARACTER_SPACING * 2)) * ((Player::CHARACTER_COUNT / CHARACTERS_PER_LINE) + 1))) / 2;
	rect_b.w = Main::graphics->player_clip[SPR_AVATAR]->w + (CHARACTER_SPACING * 2);
	rect_b.h = Main::graphics->player_clip[SPR_AVATAR]->h + (CHARACTER_SPACING * 2);

	rect.x = rect_b.x;
	rect.y = rect_b.y;
	rect.w = (Main::graphics->player_clip[SPR_AVATAR]->w + (CHARACTER_SPACING * 2)) * CHARACTERS_PER_LINE;
	rect.h = rect_b.h;

	color = 0;

	for(int i = 0; i < 4; i++) {
		if(player_random[i]) {
			if(color != 0)
				color = Graphics::combine_colors(color, Player::COLORS[i]);
			else
				color = Player::COLORS[i];
		}
	}
	
	SDL_FillRect(screen, &rect, color);

	rect_c.x = rect_b.x + CHARACTER_SPACING;
	rect_c.y = rect_b.y + CHARACTER_SPACING;
	rect_c.w = (rect_b.w * CHARACTERS_PER_LINE) - (CHARACTER_SPACING * 2);
	rect_c.h = rect_b.h - (CHARACTER_SPACING * 2);
	SDL_FillRect(screen, &rect_c, 0);

	surface = Main::graphics->text_random;
	rect.x = (WINDOW_WIDTH - surface->w) / 2;
	rect.y = rect_b.y + 16;
	SDL_BlitSurface(surface, NULL, screen, &rect);

	// Player corner
	for(int i = 0; i < 4; i++) {
		if(player_joined[i]) {
			rect_c.x = rect_b.x;
			rect_c.y = rect_b.y;
			if(i == 1 || i == 3) rect_c.x += 182;
			if(i == 2 || i == 3) rect_c.y += 26;

			rect_s.x = 16;
			rect_s.y = 0;
			rect_s.w = 26;
			rect_s.h = 26;

			if(i == 1 || i == 3) rect_s.x += 26;
			if(i == 2 || i == 3) rect_s.y += 26;

			if(player_random[i]) {
				SDL_BlitSurface(Main::graphics->common, &rect_s, screen, &rect_c);
			}
		}
	}

	// CHARACTERS
	rect_b.y += Main::graphics->player_clip[SPR_AVATAR]->h + (CHARACTER_SPACING * 2);

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

		for(int i = 0; i < 4; i++) {
			if(player_joined[i]) {
				if(player_select[i] == idx) {
					if(color != 0)
						color = Graphics::combine_colors(color, Player::COLORS[i]);
					else
						color = Player::COLORS[i];
					if(player_ready[i]) {
						color_back = Player::COLORS[i];
						color = Player::COLORS[i];
						if(flicker[i]) {
							if(flicker_frame[i] > 0x30)
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
		}

		SDL_FillRect(screen, &rect_b, color);
		rect_c.x = rect_b.x + CHARACTER_SPACING;
		rect_c.y = rect_b.y + CHARACTER_SPACING;
		rect_c.w = rect_b.w - (CHARACTER_SPACING * 2);
		rect_c.h = rect_b.h - (CHARACTER_SPACING * 2);
		SDL_FillRect(screen, &rect_c, color_back);

		SDL_BlitSurface(Main::graphics->player->at(idx), clip, screen, &rect);

		for(int i = 0; i < 4; i++) {
			if(player_joined[i]) {
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
		}

		rect_b.x += Main::graphics->player_clip[SPR_AVATAR]->w + (CHARACTER_SPACING * 2);
	}

	// PLAYERS
	SDL_Rect r_block, r_pnumber,r_playername, r_ready;
	SDL_Rect r_stats;
	SDL_Rect * clip_direction;
	int stats_w, stats_h;
	int direction;
	stats_w = 100;
	stats_h = 20;

	for(int i = 0; i < 4; i++) {
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

		if(player_joined[i]) {
			// Player animation and background
			r_block.x += 14; r_block.w -= 30;
			r_block.y += 12; r_block.h -= 24;

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
			surface = Main::graphics->playername->at(player_select[i]);
			if(direction == -1) r_playername.x -= surface->w;
			SDL_BlitSurface(surface, NULL, screen, &r_playername);

			// Player ready
			if(player_ready[i]) {
				surface = Main::graphics->text_ready;
				if(direction == -1) r_ready.x -= surface->w;
				SDL_BlitSurface(surface, NULL, screen, &r_ready);
			}

			// Stats: speed
			for(int j = 0; j <= Player::CHARACTERS[player_select[i]].speedclass; j++) {
				rect.x = r_stats.x + ((stats_w + (j * 18)) * direction);
				rect.y = r_stats.y;
				if(direction == -1) rect.x -= 18;
				SDL_BlitSurface(Main::graphics->statsblock[j], NULL, screen, &rect);
			}

			// Stats: weight
			for(int j = 0; j <= Player::CHARACTERS[player_select[i]].weightclass; j++) {
				rect.x = r_stats.x + ((stats_w + (j * 18)) * direction);
				rect.y = r_stats.y + stats_h;
				if(direction == -1) rect.x -= 18;
				SDL_BlitSurface(Main::graphics->statsblock[j], NULL, screen, &rect);
			}

			// Stats: weapon
			for(int j = 0; j <= Player::CHARACTERS[player_select[i]].weaponclass; j++) {
				rect.x = r_stats.x + ((stats_w + (j * 18)) * direction);
				rect.y = r_stats.y + (stats_h * 2);
				if(direction == -1) rect.x -= 18;
				SDL_BlitSurface(Main::graphics->statsblock[j], NULL, screen, &rect);
			}

			// Stats: bomb
			for(int j = 0; j <= Player::CHARACTERS[player_select[i]].bombpowerclass; j++) {
				rect.x = r_stats.x + ((stats_w + (j * 18)) * direction);
				rect.y = r_stats.y + (stats_h * 3);
				if(direction == -1) rect.x -= 18;
				SDL_BlitSurface(Main::graphics->statsblock[j], NULL, screen, &rect);
			}
		} else {
			r_block.x += 10; r_block.w -= 20;
			r_block.y += 10; r_block.h -= 20;
			SDL_FillRect(screen, &r_block, 0);

			if(direction == 1)
				rect.x = r_block.x + 20;
			else
				rect.x = r_block.x + r_block.w - Main::graphics->text_pressstart->w - 20;
			rect.y = r_block.y + 80;

			if(!(frame & 0x20))
				SDL_BlitSurface(Main::graphics->text_pressstart, NULL, screen, &rect);
		}
	}
}
