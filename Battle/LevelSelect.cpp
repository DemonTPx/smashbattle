#include "SDL/SDL.h"

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

#define MENU_TOP_OFFSET 128
#define TILES_COLS		10
#define TILES_ROWS		9

LevelSelect::LevelSelect() {
}

void LevelSelect::run() {
	SDL_Event event;

	load_sprites();

	input = Main::instance->input_master;

	ready = false;
	ready_level = false;
	cancel = false;

	random = false;
	random_start = 0;
	random_before = 0;

	srand(SDL_GetTicks());
	
	level = 0;
	select(DIRECTION_NONE);

	frame = 0;

	while (Main::running && !ready) {
		while(SDL_PollEvent(&event)) {
			Main::instance->handle_event(&event);
			
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
				ready = true;
				cancel = true;
				break;
			}

			input->handle_event(&event);
		}

		process_cursors();

		process_random();

		frame++;

		draw();

		if(ready_level && flicker_frame > 0x20) {
			ready = true;
		}

		Main::instance->flip();
	}

	if(!ready)
		cancel = true;

	free_sprites();
}

void LevelSelect::process_cursors() {
	int direction;

	if(input->is_pressed(A_RUN) || input->is_pressed(A_JUMP) ||
		input->is_pressed(A_SHOOT) || input->is_pressed(A_BOMB)) {
			if(!(input->is_pressed(A_JUMP) && input->is_pressed(A_UP))) { // It's likely that up and jump are the same keybind
				if(!ready_level) {
					ready_level = true;
					
					Main::audio->play(SND_SELECT_CHARACTER);

					random = false;

					flicker = true;
					flicker_frame = 0;
				}
			}
	}
	
	direction = 0;
	if(input->is_pressed(A_LEFT)) direction |= DIRECTION_LEFT;
	if(input->is_pressed(A_RIGHT)) direction |= DIRECTION_RIGHT;
	if(input->is_pressed(A_UP)) direction |= DIRECTION_UP;
	if(input->is_pressed(A_DOWN)) direction |= DIRECTION_DOWN;
	if(direction != DIRECTION_NONE) {
		if(!ready_level) {
			select(direction);
			Main::audio->play(SND_SELECT);
		}
	}
}

void LevelSelect::process_random() {
	if(random && (frame - random_start == 6)) {
		int last;
		bool is_last;

		last = level;

		do {
			is_last = false;
			level = rand() % Level::LEVEL_COUNT;

			if(level == last) is_last = true;
		} while(is_last);

		random_start = frame;
	}
}

void LevelSelect::select(int direction) {
	if(!random) {
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
	}
	if(direction & DIRECTION_UP) {
		if(random) {
			if(random_before >= Level::LEVEL_COUNT - LEVELS_PER_LINE) {
				level = random_before;
			} else {
				level = random_before - LEVELS_PER_LINE + Level::LEVEL_COUNT;
			}
			random = false;
		} else if(level < LEVELS_PER_LINE) {
			random = true;
			random_before = level;
			random_start = frame;
		} else {
			level -= LEVELS_PER_LINE;
		}
	}
	if(direction & DIRECTION_DOWN) {
		if(random) {
			if(random_before < LEVELS_PER_LINE) {
				level = random_before;
			} else {
				level = random_before + LEVELS_PER_LINE - Level::LEVEL_COUNT;
			}
			random = false;
		} else if(level >= Level::LEVEL_COUNT - LEVELS_PER_LINE) {
			random = true;
			random_before = level;
			random_start = frame;
		} else {
			level += LEVELS_PER_LINE;
		}
	}

	if(level < 0) level += Level::LEVEL_COUNT;
	if(level >= Level::LEVEL_COUNT) level -= Level::LEVEL_COUNT;
}

void LevelSelect::draw() {
	SDL_Surface * screen;
	SDL_Surface * surface;
	SDL_Rect rect, rect_b, rect_s;
	Uint32 color;
	int width;

	screen = Main::instance->screen;

	SDL_BlitSurface(backgrounds->at(level), NULL, screen, NULL);

	// TILES
	rect.x = (WINDOW_WIDTH - (TILES_COLS * TILE_W)) / 2;
	rect.y = MENU_TOP_OFFSET - 40 - TILE_H;
	rect.w = TILES_COLS * TILE_W;
	rect.h = TILES_ROWS * TILE_H;

	SDL_FillRect(screen, &rect, 0);

	rect_s.x = 0;
	rect_s.y = 0;
	rect_s.w = TILE_W;
	rect_s.h = TILE_H;

	rect.x = (WINDOW_WIDTH - (TILES_COLS * TILE_W)) / 2;
	rect.y = MENU_TOP_OFFSET - 40 - TILE_H;
	for(int i = 0; i < TILES_COLS; i++) {
		SDL_BlitSurface(Main::graphics->tiles, &rect_s, screen, &rect);
		rect.x += TILE_W;
	}

	for(int i = 1; i < TILES_ROWS; i++) {
		rect.x = (WINDOW_WIDTH - (TILES_COLS * TILE_W)) / 2;
		rect.y = (MENU_TOP_OFFSET - 40 - TILE_H) + (TILE_H * i);
		SDL_BlitSurface(Main::graphics->tiles, &rect_s, screen, &rect);
		
		rect.x = rect.x + ((TILES_COLS - 1) * TILE_W);
		SDL_BlitSurface(Main::graphics->tiles, &rect_s, screen, &rect);
	}

	rect.x = (WINDOW_WIDTH - (TILES_COLS * TILE_W)) / 2;
	rect.y = (MENU_TOP_OFFSET - 40 - TILE_H) + (TILES_ROWS * TILE_H);
	for(int i = 0; i < TILES_COLS; i++) {
		SDL_BlitSurface(Main::graphics->tiles, &rect_s, screen, &rect);
		rect.x += TILE_W;
	}

	// STAGES
	width = (LEVEL_WIDTH + (LEVEL_SPACING * 2)) * LEVELS_PER_LINE;
	
	// Stage random
	if(random) {
		rect.x = (WINDOW_WIDTH - width) / 2;
		rect.y = MENU_TOP_OFFSET - 32;
		rect.w = width;
		rect.h = 32;

		color = 0x0088ff;
		SDL_FillRect(screen, &rect, color);

		rect.x += LEVEL_SPACING;
		rect.y += LEVEL_SPACING;
		rect.w -= (LEVEL_SPACING * 2);
		rect.h -= (LEVEL_SPACING * 2);

		SDL_FillRect(screen, &rect, 0);
	}

	surface = Main::graphics->text_random;
	rect.x = (screen->w - surface->w) / 2;
	rect.y = MENU_TOP_OFFSET - 24;
	SDL_BlitSurface(surface, NULL, screen, &rect);

	// Stage thumbnails
	rect_b.x = (screen->w - width) / 2;
	rect_b.y = MENU_TOP_OFFSET;
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
			color = 0x0088ff;
			
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
	
	// Stage name
	surface = Main::text->render_text_medium(Level::LEVELS[level].name);
	rect.x = (screen->w - surface->w) / 2;
	rect.y = MENU_TOP_OFFSET + ((LEVEL_HEIGHT + (LEVEL_SPACING * 2)) * (Level::LEVEL_COUNT / LEVELS_PER_LINE)) + 10;
	SDL_BlitSurface(surface, NULL, screen, &rect);
	SDL_FreeSurface(surface);
}

void LevelSelect::load_sprites() {
	SDL_Surface * surface;

	thumbs = new std::vector<SDL_Surface*>(0);
	backgrounds = new std::vector<SDL_Surface*>(0);

	for(int idx = 0; idx < Level::LEVEL_COUNT; idx++) {
		surface = Level::get_thumbnail(Level::LEVELS[idx].filename);
		thumbs->push_back(surface);

		surface = Level::get_preview(Level::LEVELS[idx].filename);
		backgrounds->push_back(surface);
	}
}

void LevelSelect::free_sprites() {
	for(unsigned int i = 0; i < thumbs->size(); i++) {
		SDL_FreeSurface(thumbs->at(i));
	}
	delete thumbs;

	for(unsigned int i = 0; i < backgrounds->size(); i++) {
		SDL_FreeSurface(backgrounds->at(i));
	}
	delete backgrounds;
}
