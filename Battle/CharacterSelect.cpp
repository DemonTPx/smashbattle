#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"

#include "Main.h"
#include "Battle.h"
#include "Player.h"

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

CharacterSelect::CharacterSelect(Battle * parent) {
	this->parent = parent;
}

void CharacterSelect::run() {
	SDL_Event event;

	load_fonts();
	load_sprites();

	ready1 = false;
	ready2 = false;

	ready_stage = false;

	ready = false;

	controls1 = Main::instance->controls1;
	controls2 = Main::instance->controls2;
	
	cursor1_direction = DIRECTION_NONE;
	cursor1_direction_start = 0;
	cursor1_first = true;
	cursor1_enter = false;
	cursor2_direction = DIRECTION_NONE;
	cursor2_direction_start = 0;
	cursor2_first = true;
	cursor2_enter = false;

	select1 = 0;
	if(Battle::CHARACTER_COUNT >= CHARACTERS_PER_LINE)
		select2 = CHARACTERS_PER_LINE - 1;
	else
		select2 = Battle::CHARACTER_COUNT;
	
	stage = 0;
	select_stage(DIRECTION_NONE);

	ruleset = 0;

	frame = 0;

	while (Main::running && !ready) {
		while(SDL_PollEvent(&event)) {
			Main::instance->handle_event(&event);
			handle_input(&event);
		}

		process_cursors();

		name1 = Battle::characters[select1].name;
		name2 = Battle::characters[select2].name;

		file1 = Battle::characters[select1].filename;
		file2 = Battle::characters[select2].filename;

		draw();

		frame++;
		Main::instance->flip();
	}

	Main::audio->stop_music();

	free_sprites();
	free_fonts();
}

void CharacterSelect::handle_input(SDL_Event * event) {
	int old_direction1, old_direction2;
	old_direction1 = cursor1_direction;
	old_direction2 = cursor2_direction;

	if(event->type == SDL_KEYDOWN) {
		// Keyboard 1
		if(controls1.use_keyboard) {
			if(event->key.keysym.sym == controls1.kb_left)
				cursor1_direction |= DIRECTION_LEFT;
			if(event->key.keysym.sym == controls1.kb_right)
				cursor1_direction |= DIRECTION_RIGHT;
			if(event->key.keysym.sym == controls1.kb_up)
				cursor1_direction |= DIRECTION_UP;
			if(event->key.keysym.sym == controls1.kb_down)
				cursor1_direction |= DIRECTION_DOWN;
			else if(event->key.keysym.sym == controls1.kb_shoot || 
				event->key.keysym.sym == controls1.kb_run ||
				(controls1.kb_up != controls1.kb_jump &&
				event->key.keysym.sym == controls1.kb_jump)) {
					cursor1_enter = true;
			}
		}
		// Keyboard 2
		if(controls2.use_keyboard) {
			if(event->key.keysym.sym == controls2.kb_left)
				cursor2_direction |= DIRECTION_LEFT;
			if(event->key.keysym.sym == controls2.kb_right)
				cursor2_direction |= DIRECTION_RIGHT;
			if(event->key.keysym.sym == controls2.kb_up)
				cursor2_direction |= DIRECTION_UP;
			if(event->key.keysym.sym == controls2.kb_down)
				cursor2_direction |= DIRECTION_DOWN;
			else if(event->key.keysym.sym == controls2.kb_shoot || 
				event->key.keysym.sym == controls2.kb_run ||
				(controls2.kb_up != controls2.kb_jump &&
				event->key.keysym.sym == controls2.kb_jump)) {
					cursor2_enter = true;
			}
		}
	}
	if(event->type == SDL_KEYUP) {
		// Keyboard 1
		if(controls1.use_keyboard) {
			if(event->key.keysym.sym == controls1.kb_left && cursor1_direction & DIRECTION_LEFT)
				cursor1_direction ^= DIRECTION_LEFT;
			if(event->key.keysym.sym == controls1.kb_right && cursor1_direction & DIRECTION_RIGHT)
				cursor1_direction ^= DIRECTION_RIGHT;
			if(event->key.keysym.sym == controls1.kb_up && cursor1_direction & DIRECTION_UP)
				cursor1_direction ^= DIRECTION_UP;
			if(event->key.keysym.sym == controls1.kb_down && cursor1_direction & DIRECTION_DOWN)
				cursor1_direction ^= DIRECTION_DOWN;
			else if(event->key.keysym.sym == controls1.kb_shoot || 
				event->key.keysym.sym == controls1.kb_run ||
				(controls1.kb_up != controls1.kb_jump &&
				event->key.keysym.sym == controls1.kb_jump)) {
					cursor1_enter = false;
			}
		}
		// Keyboard 2
		if(controls2.use_keyboard) {
			if(event->key.keysym.sym == controls2.kb_left && cursor2_direction & DIRECTION_LEFT)
				cursor2_direction ^= DIRECTION_LEFT;
			if(event->key.keysym.sym == controls2.kb_right && cursor2_direction & DIRECTION_RIGHT)
				cursor2_direction ^= DIRECTION_RIGHT;
			if(event->key.keysym.sym == controls2.kb_up && cursor2_direction & DIRECTION_UP)
				cursor2_direction ^= DIRECTION_UP;
			if(event->key.keysym.sym == controls2.kb_down && cursor2_direction & DIRECTION_DOWN)
				cursor2_direction ^= DIRECTION_DOWN;
			else if(event->key.keysym.sym == controls2.kb_shoot || 
				event->key.keysym.sym == controls2.kb_run ||
				(controls2.kb_up != controls2.kb_jump &&
				event->key.keysym.sym == controls2.kb_jump)) {
					cursor2_enter = false;
			}
		}
	}
	if(event->type == SDL_JOYBUTTONDOWN) {
		// Joystick 1 Buttons
		if(controls1.use_joystick && event->jbutton.which == controls1.joystick_idx) {
			if(event->jbutton.button == controls1.js_left)
				cursor1_direction |= DIRECTION_LEFT;
			if(event->jbutton.button == controls1.js_right)
				cursor1_direction |= DIRECTION_RIGHT;
			if(event->jbutton.button == controls1.js_jump ||
				event->jbutton.button == controls1.js_run ||
				event->jbutton.button == controls1.js_shoot) {
					cursor1_enter = true;
			}
		}
		// Joystick 2 Buttons
		if(controls2.use_joystick && event->jbutton.which == controls2.joystick_idx) {
			if(event->jbutton.button == controls2.js_left)
				cursor2_direction |= DIRECTION_LEFT;
			if(event->jbutton.button == controls2.js_right)
				cursor2_direction |= DIRECTION_RIGHT;
			if(event->jbutton.button == controls2.js_jump ||
				event->jbutton.button == controls2.js_run ||
				event->jbutton.button == controls2.js_shoot) {
					cursor2_enter = true;
			}
		}
	}
	if(event->type == SDL_JOYBUTTONUP) {
		// Joystick 1 Buttons
		if(controls1.use_joystick && event->jbutton.which == controls1.joystick_idx) {
			if(event->jbutton.button == controls1.js_left && cursor1_direction & DIRECTION_LEFT)
				cursor1_direction ^= DIRECTION_LEFT;
			if(event->jbutton.button == controls1.js_right && cursor1_direction & DIRECTION_RIGHT)
				cursor1_direction ^= DIRECTION_RIGHT;
		}
		// Joystick 2 Buttons
		if(controls2.use_joystick && event->jbutton.which == controls2.joystick_idx) {
			if(event->jbutton.button == controls2.js_left && cursor2_direction & DIRECTION_LEFT)
				cursor2_direction ^= DIRECTION_LEFT;
			if(event->jbutton.button == controls2.js_right && cursor2_direction & DIRECTION_RIGHT)
				cursor2_direction ^= DIRECTION_RIGHT;
		}
	}
	if(event->type == SDL_JOYAXISMOTION) {
		// Joystick 1 Axis
		if(controls1.use_joystick && event->jbutton.which == controls1.joystick_idx) {
			if(event->jaxis.axis == 0) {
				if(event->jaxis.value < -Main::JOYSTICK_AXIS_THRESHOLD)
					cursor1_direction |= DIRECTION_LEFT;
				else if(event->jaxis.value > Main::JOYSTICK_AXIS_THRESHOLD)
					cursor1_direction |= DIRECTION_RIGHT;
				else {
					if(cursor1_direction & DIRECTION_LEFT)
						cursor1_direction ^= DIRECTION_LEFT;
					if(cursor1_direction & DIRECTION_RIGHT)
						cursor1_direction ^= DIRECTION_RIGHT;
				}
			} 
			if(event->jaxis.axis == 1) {
				if(event->jaxis.value < -Main::JOYSTICK_AXIS_THRESHOLD)
					cursor1_direction |= DIRECTION_UP;
				else if(event->jaxis.value > Main::JOYSTICK_AXIS_THRESHOLD)
					cursor1_direction |= DIRECTION_DOWN;
				else {
					if(cursor1_direction & DIRECTION_UP)
						cursor1_direction ^= DIRECTION_UP;
					if(cursor1_direction & DIRECTION_DOWN)
						cursor1_direction ^= DIRECTION_DOWN;
				}
			}
		}
		// Joystick 2 Axis
		if(controls2.use_joystick && event->jbutton.which == controls2.joystick_idx) {
			if(event->jaxis.axis == 0) {
				if(event->jaxis.value < -Main::JOYSTICK_AXIS_THRESHOLD)
					cursor2_direction |= DIRECTION_LEFT;
				else if(event->jaxis.value > Main::JOYSTICK_AXIS_THRESHOLD)
					cursor2_direction |= DIRECTION_RIGHT;
				else {
					if(cursor2_direction & DIRECTION_LEFT)
						cursor2_direction ^= DIRECTION_LEFT;
					if(cursor2_direction & DIRECTION_RIGHT)
						cursor2_direction ^= DIRECTION_RIGHT;
				}
			}
			if(event->jaxis.axis == 1) {
				if(event->jaxis.value < -Main::JOYSTICK_AXIS_THRESHOLD)
					cursor2_direction |= DIRECTION_UP;
				else if(event->jaxis.value > Main::JOYSTICK_AXIS_THRESHOLD)
					cursor2_direction |= DIRECTION_DOWN;
				else {
					if(cursor2_direction & DIRECTION_UP)
						cursor2_direction ^= DIRECTION_UP;
					if(cursor2_direction & DIRECTION_DOWN)
						cursor2_direction ^= DIRECTION_DOWN;
				}
			}
		}
	}

	if(cursor1_direction != old_direction1) {
		cursor1_first = true;
	}
	if(cursor2_direction != old_direction2) {
		cursor2_first = true;
	}
}

void CharacterSelect::process_cursors() {
	int delay;

	if(cursor1_enter) {
		cursor1_enter = false;
		if(ready1 && ready2) {
			Main::audio->play(SND_SELECT_CHARACTER);
			if(!ready_stage) {
				ready_stage = true;
				flicker_stage = true;
				flicker_stage_frame = 0;
			} else {
				ready = true;
			}
		}
		if(!ready1) {
			ready1 = true;
			flicker1 = true;
			flicker1_frame = 0;
			Main::audio->play(SND_SELECT_CHARACTER);
		}
	}

	if(cursor1_direction != 0) {
		if(cursor1_first)
			delay = 0;
		else
			delay = Main::CONTROLS_REPEAT_SPEED;
		if(frame - cursor1_direction_start > delay) {
			cursor1_direction_start = frame;
			cursor1_first = false;
			if(!ready1) {
				select(&select1, cursor1_direction);
				Main::audio->play(SND_SELECT);
			}
			if(ready1 && ready2) {
					Main::audio->play(SND_SELECT);
				if(!ready_stage) {
					select_stage(cursor1_direction);
				} else {
					select_ruleset(cursor1_direction);
				}
			}
		}
	}

	if(cursor2_enter) {
		cursor2_enter = false;
		if(ready1 && ready2) {
			Main::audio->play(SND_SELECT_CHARACTER);
			if(!ready_stage) {
				ready_stage = true;
				flicker_stage = true;
				flicker_stage_frame = 0;
			} else {
				ready = true;
			}
		}
		if(!ready2) {
			ready2 = true;
			flicker2 = true;
			flicker2_frame = 0;
			Main::audio->play(SND_SELECT_CHARACTER);
		}
	}

	if(cursor2_direction != DIRECTION_NONE) {
		if(cursor2_first)
			delay = 0;
		else
			delay = Main::CONTROLS_REPEAT_SPEED;
		if(frame - cursor2_direction_start > delay) {
			cursor2_direction_start = frame;
			cursor2_first = false;
			if(!ready2) {
				select(&select2, cursor2_direction);
				Main::audio->play(SND_SELECT);
			}
			if(ready1 && ready2) {
				Main::audio->play(SND_SELECT);
				if(!ready_stage) {
					select_stage(cursor2_direction);
				} else {
					select_ruleset(cursor2_direction);
				}
			}
		}
	}
}

void CharacterSelect::select(int * select, int direction) {
	int * other;

	other = (select == &select1) ? &select2 : &select1;

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

	while(*select < Battle::CHARACTER_COUNT) {
		*select += Battle::CHARACTER_COUNT;
	}
	while(*select >= Battle::CHARACTER_COUNT) {
		*select -= Battle::CHARACTER_COUNT;
	}

	if(*select == *other) {
		this->select(select, direction);
	}
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

	if(stage < 0) stage += Battle::STAGE_COUNT;
	if(stage >= Battle::STAGE_COUNT) stage -= Battle::STAGE_COUNT;

	stage_name = Battle::stages[stage].name;
	stage_author = Battle::stages[stage].author;
}

void CharacterSelect::select_ruleset(int direction) {
	if(direction & DIRECTION_UP) {
		ruleset--;
	}
	if(direction & DIRECTION_DOWN) {
		ruleset++;
	}

	if(ruleset < 0) ruleset += Battle::RULESET_COUNT;
	if(ruleset >= Battle::RULESET_COUNT) ruleset -= Battle::RULESET_COUNT;
}

void CharacterSelect::draw() {
	SDL_Surface * screen;
	SDL_Surface * surface;
	SDL_Rect rect, rect_b, rect_s;
	SDL_Rect * clip;
	Uint32 color;

	screen = Main::instance->screen;

	SDL_FillRect(screen, NULL, 0);

	// PLAYERS

	rect_b.x = (screen->clip_rect.w - ((clip_avatar->w + (CHARACTER_SPACING * 2)) * CHARACTERS_PER_LINE)) / 2;
	rect_b.y = 20;
	rect_b.w = clip_avatar->w + (CHARACTER_SPACING * 2);
	rect_b.h = clip_avatar->h + (CHARACTER_SPACING * 2);

	for(int idx = 0; idx < Battle::CHARACTER_COUNT; idx++) {
		if(idx > 0 && idx % CHARACTERS_PER_LINE == 0) {
			rect_b.x = (screen->clip_rect.w - ((clip_avatar->w + (CHARACTER_SPACING * 2)) * CHARACTERS_PER_LINE)) / 2;
			rect_b.y += rect_b.h;
		}

		rect.x = rect_b.x + CHARACTER_SPACING;
		rect.y = rect_b.y + CHARACTER_SPACING;

		clip = clip_avatar;

		color = 0;

		if(select1 == idx) {
			color = 0xff0000;
			if(ready1 && flicker1) {
				if(flicker1_frame > 0x20)
					flicker1 = false;
				if(flicker1_frame & 0x4)
					color = 0xffffff;
				flicker1_frame++;
			}
			if(ready1) clip = clip_avatar_selected;
		}
		if(select2 == idx) {
			color = 0x0000ff;
			if(ready2 && flicker2) {
				if(flicker2_frame > 0x20)
					flicker2 = false;
				if(flicker2_frame & 0x4)
					color = 0xffffff;
				flicker2_frame++;
			}
			if(ready2) clip = clip_avatar_selected;
		}
		SDL_FillRect(screen, &rect_b, color);

		SDL_BlitSurface(character_sprites->at(idx), clip, screen, &rect);

		rect_b.x += clip_avatar->w + (CHARACTER_SPACING * 2);
	}

	// Player 1
	rect.x = 40;
	rect.y = 40;
	SDL_BlitSurface(character_sprites->at(select1), clip_left, screen, &rect);

	surface = TTF_RenderText_Solid(font26, name1, fontColor);
	rect.x = 50 + PLAYER_W;
	rect.y = 45;
	SDL_BlitSurface(surface, NULL, screen, &rect);
	SDL_FreeSurface(surface);

	if(ready1) {
		surface = TTF_RenderText_Solid(font26, "READY", fontColor);
		rect.x = 50 + PLAYER_W;
		rect.y = 65;
		SDL_BlitSurface(surface, NULL, screen, &rect);
		SDL_FreeSurface(surface);
	}

	// Player 2
	rect.x = screen->clip_rect.w - PLAYER_W - 40;
	rect.y = 40;
	SDL_BlitSurface(character_sprites->at(select2), clip_right, screen, &rect);

	surface = TTF_RenderText_Solid(font26, name2, fontColor);
	rect.x = screen->clip_rect.w - PLAYER_W - surface->clip_rect.w - 50;
	rect.y = 45;
	SDL_BlitSurface(surface, NULL, screen, &rect);
	SDL_FreeSurface(surface);

	if(ready2) {
		surface = TTF_RenderText_Solid(font26, "READY", fontColor);
		rect.x = screen->clip_rect.w - PLAYER_W - surface->clip_rect.w - 50;
		rect.y = 65;
		SDL_BlitSurface(surface, NULL, screen, &rect);
		SDL_FreeSurface(surface);
	}
	
	// STAGES

	surface = TTF_RenderText_Solid(font26, stage_name, fontColor);
	rect.x = (screen->w - surface->w) / 2;
	rect.y = 160;
	SDL_BlitSurface(surface, NULL, screen, &rect);
	SDL_FreeSurface(surface);

	rect_b.x = (screen->w - ((STAGE_WIDTH + (STAGE_SPACING * 2)) * STAGES_PER_LINE)) / 2;
	rect_b.y = 180;
	rect_b.w = STAGE_WIDTH + (STAGE_SPACING * 2);
	rect_b.h = STAGE_HEIGHT + (STAGE_SPACING * 2);

	for(int idx = 0; idx < Battle::STAGE_COUNT; idx++) {
		if(idx > 0 && idx % STAGES_PER_LINE == 0) {
			rect_b.x = (screen->w - ((STAGE_WIDTH + (STAGE_SPACING * 2)) * STAGES_PER_LINE)) / 2;
			rect_b.y += rect_b.h;
		}

		rect.x = rect_b.x + STAGE_SPACING;
		rect.y = rect_b.y + STAGE_SPACING;

		color = 0;

		if((ready1 && ready2) && stage == idx) {
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

	// RULESETS
	RuleSet ruleset;
	char text[5];
	int left, top;
	for(int i = 0; i < Battle::RULESET_COUNT; i++) {
		top = 350 + (i * 20);
		left = 55;
		
		if(ready_stage && i == this->ruleset) {
			rect.x = left - 2;
			rect.y = top - 3;
			rect.w = screen->w - (left * 2) + 4;
			rect.h = 22;
			SDL_FillRect(screen, &rect, 0x444488);
		}

		ruleset = Battle::rulesets[i];
		surface = TTF_RenderText_Solid(font26, ruleset.name, fontColor);
		rect.x = left; rect.y = top;
		SDL_BlitSurface(surface, NULL, screen, &rect);
		SDL_FreeSurface(surface);
		left += 150;

		// bullets
		rect.x = left; rect.y = top + 6;
		rect_s.x = 0; rect_s.y = 0; rect_s.w = 8; rect_s.h = 8;
		SDL_BlitSurface(parent->weapons, &rect_s, screen, &rect);
		left += 20;

		strcpy(text, "-\0");
		if(ruleset.bullets > -1) sprintf_s(text, 5, "%d", ruleset.bullets);
		surface = TTF_RenderText_Solid(font26, text, fontColor);
		rect.x = left; rect.y = top;
		SDL_BlitSurface(surface, NULL, screen, &rect);
		SDL_FreeSurface(surface);
		left += 50;

		// doubledamage bullets
		rect.x = left; rect.y = top + 6;
		rect_s.x = 8; rect_s.y = 0; rect_s.w = 8; rect_s.h = 8;
		SDL_BlitSurface(parent->weapons, &rect_s, screen, &rect);
		left += 20;

		strcpy(text, "-\0");
		if(ruleset.doubledamagebullets > -1) sprintf_s(text, 5, "%d", ruleset.doubledamagebullets);
		surface = TTF_RenderText_Solid(font26, text, fontColor);
		rect.x = left; rect.y = top;
		SDL_BlitSurface(surface, NULL, screen, &rect);
		SDL_FreeSurface(surface);
		left += 50;

		// instantkill bullets
		rect.x = left; rect.y = top + 6;
		rect_s.x = 16; rect_s.y = 0; rect_s.w = 8; rect_s.h = 8;
		SDL_BlitSurface(parent->weapons, &rect_s, screen, &rect);
		left += 20;

		strcpy(text, "-\0");
		if(ruleset.instantkillbullets > -1) sprintf_s(text, 5, "%d", ruleset.instantkillbullets);
		surface = TTF_RenderText_Solid(font26, text, fontColor);
		rect.x = left; rect.y = top;
		SDL_BlitSurface(surface, NULL, screen, &rect);
		SDL_FreeSurface(surface);
		left += 50;

		// bombs
		rect.x = left; rect.y = top;
		rect_s.x = 16; rect_s.y = 0; rect_s.w = 16; rect_s.h = 16;
		SDL_BlitSurface(parent->surface_bombs, &rect_s, screen, &rect);
		left += 20;

		strcpy(text, "-\0");
		if(ruleset.bombs > -1) sprintf_s(text, 5, "%d", ruleset.bombs);
		surface = TTF_RenderText_Solid(font26, text, fontColor);
		rect.x = left; rect.y = top;
		SDL_BlitSurface(surface, NULL, screen, &rect);
		SDL_FreeSurface(surface);
		left += 50;

		// powerups
		if(ruleset.healthpowerups > 0) {
			rect.x = left; rect.y = top;
			rect_s.x = 0; rect_s.y = 0; rect_s.w = 16; rect_s.h = 16;
			SDL_BlitSurface(parent->powerup, &rect_s, screen, &rect);
			left += 20;
		}
		if(ruleset.bulletpowerups > 0) {
			rect.x = left; rect.y = top;
			rect_s.x = 32; rect_s.y = 0; rect_s.w = 16; rect_s.h = 16;
			SDL_BlitSurface(parent->powerup, &rect_s, screen, &rect);
			left += 20;
		}
		if(ruleset.doubledamagepowerups > 0) {
			rect.x = left; rect.y = top;
			rect_s.x = 48; rect_s.y = 0; rect_s.w = 16; rect_s.h = 16;
			SDL_BlitSurface(parent->powerup, &rect_s, screen, &rect);
			left += 20;
		}
		if(ruleset.instantkillpowerups > 0) {
			rect.x = left; rect.y = top;
			rect_s.x = 64; rect_s.y = 0; rect_s.w = 16; rect_s.h = 16;
			SDL_BlitSurface(parent->powerup, &rect_s, screen, &rect);
			left += 20;
		}
		if(ruleset.bombpowerups > 0) {
			rect.x = left; rect.y = top;
			rect_s.x = 16; rect_s.y = 0; rect_s.w = 16; rect_s.h = 16;
			SDL_BlitSurface(parent->powerup, &rect_s, screen, &rect);
			left += 20;
		}
	}
}

void CharacterSelect::load_fonts() {
	font26 = TTF_OpenFont("fonts/slick.ttf", 26);
	font13 = TTF_OpenFont("fonts/slick.ttf", 13);
	fontColor.r = 255;
	fontColor.g = 255;
	fontColor.b = 255;
}

void CharacterSelect::free_fonts() {
	TTF_CloseFont(font26);
	TTF_CloseFont(font13);
}

void CharacterSelect::load_sprites() {
	SDL_Surface * loaded;
	SDL_Surface * sprites;
	Uint32 colorkey;

	character_sprites = new std::vector<SDL_Surface*>(0);

	for(int idx = 0; idx < Battle::CHARACTER_COUNT; idx++) {
		loaded = SDL_LoadBMP(Battle::characters[idx].filename);
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

	for(int idx = 0; idx < Battle::STAGE_COUNT; idx++) {
		sprites = Battle::create_level_thumbnail(Battle::stages[idx].filename);
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
