#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"

#include "Main.h"
#include "Player.h"

#include <vector>

#include "CharacterSelect.h"

#ifndef WIN32
#define sprintf_s snprintf
#endif

#define CHARACTERS_PER_LINE	4
#define CHARACTER_WIDTH		44
#define CHARACTER_SPACING	4

#define STAGES_PER_LINE	4
#define STAGE_WIDTH		44
#define STAGE_HEIGHT	34
#define STAGE_SPACING	4

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

	load_sprites();

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

	ready_stage = false;

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
	
	stage = 0;
	select_stage(DIRECTION_NONE);

	frame = 0;

	while (Main::running && !ready) {
		while(SDL_PollEvent(&event)) {
			Main::instance->handle_event(&event);
			handle_input(&event);
		}

		process_cursors();

		for(int i = 0; i < players; i++) {
			name[i] = Player::CHARACTERS[player_select[i]].name;
			file[i] = Player::CHARACTERS[player_select[i]].filename;
		}

		draw();

		frame++;
		Main::instance->flip();
	}

	Main::audio->stop_music();

	free_sprites();
}

void CharacterSelect::handle_input(SDL_Event * event) {
	int old_direction[4];

	for(int i = 0; i < players; i++) {
		old_direction[i] = cursor_direction[i];

		if(event->type == SDL_KEYDOWN) {
			// Escape key always returns to main menu
			if(event->key.keysym.sym == SDLK_ESCAPE) {
				ready = true;
				cancel = true;
			}

			// Keyboard 1
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
			// Keyboard 1
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
		if(event->type == SDL_JOYBUTTONDOWN) {
			// Joystick 1 Buttons
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
			// Joystick 1 Buttons
			if(controls[i].use_joystick && event->jbutton.which == controls[i].joystick_idx) {
				if(event->jbutton.button == controls[i].js_left && cursor_direction[i] & DIRECTION_LEFT)
					cursor_direction[i] ^= DIRECTION_LEFT;
				if(event->jbutton.button == controls[i].js_right && cursor_direction[i] & DIRECTION_RIGHT)
					cursor_direction[i] ^= DIRECTION_RIGHT;
			}
		}
		if(event->type == SDL_JOYAXISMOTION) {
			// Joystick 1 Axis
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
			if(players_ready) {
				Main::audio->play(SND_SELECT_CHARACTER);
				if(!ready_stage) {
					ready_stage = true;
					flicker_stage = true;
					flicker_stage_frame = 0;
					ready = true;
				} else {
					ready = true;
				}
			}
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
					Main::audio->play(SND_SELECT);
				}
				if(players_ready) {
						Main::audio->play(SND_SELECT);
					if(!ready_stage) {
						select_stage(cursor_direction[i]);
					}
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

void CharacterSelect::select_stage(int direction) {
	if(direction & DIRECTION_LEFT) {
		if(stage % STAGES_PER_LINE == 0)
			stage += STAGES_PER_LINE;
		stage--;
	}
	if(direction & DIRECTION_RIGHT) {
		if(stage % STAGES_PER_LINE == STAGES_PER_LINE - 1)
			stage -= STAGES_PER_LINE;
		stage++;
	}
	if(direction & DIRECTION_UP) {
		stage -= STAGES_PER_LINE;
	}
	if(direction & DIRECTION_DOWN) {
		stage += STAGES_PER_LINE;
	}

	if(stage < 0) stage += Level::LEVEL_COUNT;
	if(stage >= Level::LEVEL_COUNT) stage -= Level::LEVEL_COUNT;

	stage_name = Level::LEVELS[stage].name;
}

void CharacterSelect::draw() {
	SDL_Surface * screen;
	SDL_Surface * surface;
	SDL_Surface * statsblock[3];
	SDL_Rect rect, rect_b, rect_c, rect_s;
	SDL_Rect * clip;
	Uint32 color, color_back;

	screen = Main::instance->screen;

	SDL_FillRect(screen, NULL, 0);

	// CHARACTERS

	rect_b.x = (screen->clip_rect.w - ((clip_avatar->w + (CHARACTER_SPACING * 2)) * CHARACTERS_PER_LINE)) / 2;
	rect_b.y = (screen->clip_rect.h - ((clip_avatar->h + (CHARACTER_SPACING * 2)) * (Player::CHARACTER_COUNT / CHARACTERS_PER_LINE))) / 2;
	rect_b.w = clip_avatar->w + (CHARACTER_SPACING * 2);
	rect_b.h = clip_avatar->h + (CHARACTER_SPACING * 2);

	for(int idx = 0; idx < Player::CHARACTER_COUNT; idx++) {
		if(idx > 0 && idx % CHARACTERS_PER_LINE == 0) {
			rect_b.x = (screen->clip_rect.w - ((clip_avatar->w + (CHARACTER_SPACING * 2)) * CHARACTERS_PER_LINE)) / 2;
			rect_b.y += rect_b.h;
		}

		rect.x = rect_b.x + CHARACTER_SPACING;
		rect.y = rect_b.y + CHARACTER_SPACING;

		clip = clip_avatar;

		color = 0;
		color_back = 0;

		for(int i = 0; i < players; i++) {
			if(player_select[i] == idx) {
				color = Player::COLORS[i];
				if(player_ready[i]) {
					color_back = 0xffffff;
					if(flicker[i]) {
						if(flicker_frame[i] > 0x20)
							flicker[i] = false;
						if(flicker_frame[i] & 0x4)
							color = 0xffffff;
						flicker_frame[i]++;
					}
				}
				if(player_ready[i]) clip = clip_avatar_selected;
			}
		}

		SDL_FillRect(screen, &rect_b, color);
		rect_c.x = rect_b.x + 4;
		rect_c.y = rect_b.y + 4;
		rect_c.w = rect_b.w - 8;
		rect_c.h = rect_b.h - 8;
		SDL_FillRect(screen, &rect_c, color_back);

		SDL_BlitSurface(character_sprites->at(idx), clip, screen, &rect);

		for(int i = 0; i < players; i++) {
			rect_c.x = rect_b.x + 4 + (10 * i);
			rect_c.y = rect_b.y + 4;
			rect_s.x = 16 + (10 * i);
			rect_s.y = 0;
			rect_s.w = 10;
			rect_s.h = 16;
			if(player_select[i] == idx) {
				SDL_BlitSurface(Main::graphics->common, &rect_s, screen, &rect_c);
			}
		}

		rect_b.x += clip_avatar->w + (CHARACTER_SPACING * 2);
	}

	// Player stats blocks
	statsblock[0] = SDL_CreateRGBSurface(NULL, 16, 18, 32, 0, 0, 0, 0);
	SDL_FillRect(statsblock[0], NULL, 0x880000);

	statsblock[1] = SDL_CreateRGBSurface(NULL, 16, 18, 32, 0, 0, 0, 0);
	SDL_FillRect(statsblock[1], NULL, 0x888800);

	statsblock[2] = SDL_CreateRGBSurface(NULL, 16, 18, 32, 0, 0, 0, 0);
	SDL_FillRect(statsblock[2], NULL, 0x008800);

	// PLAYERS
	SDL_Rect r_block, r_avatar, r_playername, r_ready;
	SDL_Rect r_stats;
	SDL_Rect * clip_direction;
	int stats_w, stats_h;
	int direction;
	stats_w = 110;
	stats_h = 20;

	for(int i = 0; i < players; i++) {
		r_block.w = 190;
		r_block.h = 180;
		switch(i) {
			case 0:
				r_block.x = 10;
				r_block.y = 10;
				clip_direction = clip_left;
				r_avatar.x = 40;
				r_avatar.y = 20;
				r_playername.x = 50 + PLAYER_W;
				r_playername.y = 25;
				r_ready.x = 50 + PLAYER_W;
				r_ready.y = 45;
				r_stats.x = 20;
				r_stats.y = 100;
				direction = 1;
				break;
			case 1:
				r_block.x = 440;
				r_block.y = 10;
				clip_direction = clip_right;
				r_avatar.x = 600 - PLAYER_W;
				r_avatar.y = 20;
				r_playername.x = 590 - PLAYER_W;
				r_playername.y = 25;
				r_ready.x = 590 - PLAYER_W;
				r_ready.y = 45;
				r_stats.x = 620;
				r_stats.y = 100;
				direction = -1;
				break;
			case 2:
				r_block.x = 10;
				r_block.y = 290;
				clip_direction = clip_left;
				r_avatar.x = 40;
				r_avatar.y = 300;
				r_playername.x = 50 + PLAYER_W;
				r_playername.y = 305;
				r_ready.x = 50 + PLAYER_W;
				r_ready.y = 325;
				r_stats.x = 20;
				r_stats.y = 380;
				direction = 1;
				break;
			case 3:
				r_block.x = 440;
				r_block.y = 290;
				clip_direction = clip_right;
				r_avatar.x = 600 - PLAYER_W;
				r_avatar.y = 300;
				r_playername.x = 590 - PLAYER_W;
				r_playername.y = 305;
				r_ready.x = 590 + PLAYER_W;
				r_ready.y = 325;
				r_stats.x = 620;
				r_stats.y = 380;
				direction = -1;
				break;
		}
		SDL_FillRect(screen, &r_block, Player::COLORS[i]);
		r_block.x += 2; r_block.w -= 4;
		r_block.y += 2; r_block.h -= 4;
		SDL_FillRect(screen, &r_block, 0x222222);

		SDL_BlitSurface(character_sprites->at(player_select[i]), clip_direction, screen, &r_avatar);

		surface = TTF_RenderText_Solid(Main::graphics->font26, name[i], Main::graphics->white);
		if(direction == -1) r_playername.x -= surface->w;
		SDL_BlitSurface(surface, NULL, screen, &r_playername);
		SDL_FreeSurface(surface);

		if(player_ready[i]) {
			surface = TTF_RenderText_Solid(Main::graphics->font26, "READY", Main::graphics->white);
			if(direction == -1) r_ready.x -= surface->w;
			SDL_BlitSurface(surface, NULL, screen, &r_ready);
			SDL_FreeSurface(surface);
		}

		// Stats
		surface = TTF_RenderText_Solid(Main::graphics->font26, "SPEED", Main::graphics->white);
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

		surface = TTF_RenderText_Solid(Main::graphics->font26, "WEIGHT", Main::graphics->white);
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

		surface = TTF_RenderText_Solid(Main::graphics->font26, "WEAPON", Main::graphics->white);
		rect.x = r_stats.x;
		rect.y = r_stats.y + (stats_h * 2);
		if(direction == -1) rect.x -= surface->w;
		SDL_BlitSurface(surface, NULL, screen, &rect);
		SDL_FreeSurface(surface);

		for(int j = 0; j <= Player::CHARACTERS[player_select[i]].bulletrateclass; j++) {
			rect.x = r_stats.x + ((stats_w + (j * 18)) * direction);
			rect.y = r_stats.y + (stats_h * 2);
			if(direction == -1) rect.x -= 18;
			SDL_BlitSurface(statsblock[j], NULL, screen, &rect);
		}

		surface = TTF_RenderText_Solid(Main::graphics->font26, "BOMB", Main::graphics->white);
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
	
	// STAGES
	
	bool players_ready;

	players_ready = true;
	for(int i = 0; i < players; i++) {
		if(!player_ready[i])
			players_ready = false;
	}

	surface = TTF_RenderText_Solid(Main::graphics->font26, stage_name, Main::graphics->white);
	rect.x = (screen->w - surface->w) / 2;
	rect.y = 320;
	SDL_BlitSurface(surface, NULL, screen, &rect);
	SDL_FreeSurface(surface);

	rect_b.x = (screen->w - ((STAGE_WIDTH + (STAGE_SPACING * 2)) * STAGES_PER_LINE)) / 2;
	rect_b.y = 340;
	rect_b.w = STAGE_WIDTH + (STAGE_SPACING * 2);
	rect_b.h = STAGE_HEIGHT + (STAGE_SPACING * 2);

	for(int idx = 0; idx < Level::LEVEL_COUNT; idx++) {
		if(idx > 0 && idx % STAGES_PER_LINE == 0) {
			rect_b.x = (screen->w - ((STAGE_WIDTH + (STAGE_SPACING * 2)) * STAGES_PER_LINE)) / 2;
			rect_b.y += rect_b.h;
		}

		rect.x = rect_b.x + STAGE_SPACING;
		rect.y = rect_b.y + STAGE_SPACING;

		color = 0;

		if(players_ready && stage == idx) {
			color = 0x444488;
			
			if(ready_stage && flicker_stage) {
				if(flicker_stage_frame > 0x20)
					flicker_stage = false;
				if(flicker_stage_frame & 0x4)
					color = 0xffffff;
				flicker_stage_frame++;
			}
		}
		SDL_FillRect(screen, &rect_b, color);

		SDL_BlitSurface(stage_thumbs->at(idx), NULL, screen, &rect);

		rect_b.x += STAGE_WIDTH + (STAGE_SPACING * 2);
	}
}

void CharacterSelect::load_sprites() {
	SDL_Surface * loaded;
	SDL_Surface * sprites;
	Uint32 colorkey;

	character_sprites = new std::vector<SDL_Surface*>(0);

	for(int idx = 0; idx < Player::CHARACTER_COUNT; idx++) {
		loaded = SDL_LoadBMP(Player::CHARACTERS[idx].filename);
		sprites = SDL_DisplayFormat(loaded);
		SDL_FreeSurface(loaded);
		colorkey = SDL_MapRGB(sprites->format, 0, 255, 255);
		SDL_SetColorKey(sprites, SDL_SRCCOLORKEY, colorkey);
		
		character_sprites->push_back(sprites);
	}

	clip_avatar = new SDL_Rect();
	clip_avatar->x = 220;
	clip_avatar->y = 0;
	clip_avatar->w = 44;
	clip_avatar->h = 44;

	clip_avatar_selected = new SDL_Rect();
	clip_avatar_selected->x = 220;
	clip_avatar_selected->y = 44;
	clip_avatar_selected->w = 44;
	clip_avatar_selected->h = 44;
	
	clip_left = new SDL_Rect();
	clip_left->x = 0;
	clip_left->y = 0;
	clip_left->w = PLAYER_W;
	clip_left->h = PLAYER_H;
	
	clip_right = new SDL_Rect();
	clip_right->x = 0;
	clip_right->y = PLAYER_H;
	clip_right->w = PLAYER_W;
	clip_right->h = PLAYER_H;
	

	stage_thumbs = new std::vector<SDL_Surface*>(0);

	for(int idx = 0; idx < Level::LEVEL_COUNT; idx++) {
		sprites = Level::get_thumbnail(Level::LEVELS[idx].filename);
		stage_thumbs->push_back(sprites);
	}
}

void CharacterSelect::free_sprites() {
	for(unsigned int idx = 0; idx < character_sprites->size(); idx++) {
		SDL_FreeSurface(character_sprites->at(idx));
		character_sprites->erase(character_sprites->begin() + idx);
	}
	delete character_sprites;

	for(unsigned int idx = 0; idx < stage_thumbs->size(); idx++) {
		SDL_FreeSurface(stage_thumbs->at(idx));
		stage_thumbs->erase(stage_thumbs->begin() + idx);
	}
	delete stage_thumbs;

	delete clip_avatar;
	delete clip_avatar_selected;
	delete clip_left;
	delete clip_right;
}
