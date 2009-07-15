#include "SDL/SDL.h"

#include <iostream>
#include <fstream>

#include "Main.h"
#include "Gameplay.h"
#include "Level.h"

const int Level::LEVEL_COUNT = 8;
const LevelInfo Level::LEVELS[Level::LEVEL_COUNT] = {
	{(char*)"Training Dojo", (char*)"stage/trainingdojo.stg"},
	{(char*)"Common Grounds", (char*)"stage/commongrounds.stg"},
	{(char*)"Platform Alley", (char*)"stage/platformalley.stg"},
	{(char*)"Pogostick", (char*)"stage/pogostick.stg"},
	{(char*)"Pitfall", (char*)"stage/pitfall.stg"},
	{(char*)"Blast Bowl", (char*)"stage/blastbowl.stg"},
	{(char*)"Pit of Death", (char*)"stage/pitofdeath.stg"},
	{(char*)"Duck and Hunt", (char*)"stage/ducknhunt.stg"}
};

Level::Level() {
	background = NULL;
	tiles = NULL;
}

Level::~Level() {
	if(background != NULL)
		SDL_FreeSurface(background);
	if(tiles != NULL)
		SDL_FreeSurface(tiles);
}

void Level::load(char * filename) {
	std::ifstream ifs;
	char name[20], author[20], tiles_file[30], bg_file[30];
	char tiles_file_full[35], bg_file_full[35];
	char l[1];
	SDL_Surface * surface;
	Uint32 colorkey;

	ifs.open(filename, std::ifstream::binary);

	ifs.read(name, 20);
	ifs.read(author, 20);
	ifs.read(tiles_file, 30);
	ifs.read(bg_file, 30);

	// Get the level tiles (HP & sprite index)
	for(int i = 0; i < TILE_COUNT; i++) {
		ifs.read(l, 1);
		level_start[i] = (int)l[0] & 0x0f;

		if(level_start[i] == 0xf)
			level_start[i] = -1;

		level_hp_start[i] = 10 * (((int)l[0] & 0xf0) >> 4);

		if(level_hp_start[i] == 150)
			level_hp_start[i] = 10000;
	}

	ifs.close();

	strncpy(tiles_file_full, "gfx/\0", 5);
	strncat(tiles_file_full, tiles_file, 30);

	surface = SDL_LoadBMP(tiles_file_full);
	tiles = SDL_DisplayFormat(surface);
	colorkey = SDL_MapRGB(tiles->format, 0, 255, 255);
	SDL_SetColorKey(tiles, SDL_SRCCOLORKEY, colorkey);
	SDL_FreeSurface(surface);

	if(bg_file[0] != 0) {
		strncpy(bg_file_full, "gfx/\0", 5);
		strncat(bg_file_full, bg_file, 30);

		surface = SDL_LoadBMP(bg_file_full);
		background = SDL_DisplayFormat(surface);
		SDL_FreeSurface(surface);
	} else {
		background = NULL;
	}
}

LevelInformation * Level::get_information(const char *filename) {
	std::ifstream ifs;
	LevelInformation * info;
	info = new LevelInformation();
	char name[20], author[20], tiles_file[30], bg_file[30];

	ifs.open(filename, std::ifstream::binary);
	
	ifs.read(name, 20);
	ifs.read(author, 20);
	ifs.read(tiles_file, 30);
	ifs.read(bg_file, 30);

	info->name = name;
	info->author = author;
	info->filename = (char*)filename;
	info->filename_tiles = tiles_file;
	info->filename_background = bg_file;

	ifs.close();

	return info;
}

SDL_Surface * Level::get_thumbnail(const char * filename) {
	std::ifstream ifs;
	char name[20], author[20], tiles_file[30], bg_file[30];
	char l[1];
	SDL_Surface * surface;
	SDL_Rect rect;
	Uint32 fillColor;
	int maxx;

	ifs.open(filename, std::ifstream::binary);
	
	ifs.read(name, 20);
	ifs.read(author, 20);
	ifs.read(tiles_file, 30);
	ifs.read(bg_file, 30);

	surface = SDL_CreateRGBSurface(NULL, TILE_COLS * 2 + 4, TILE_ROWS * 2 + 4, 32, 0, 0, 0, 0);
	SDL_FillRect(surface, NULL, 0x888888);
	rect.x = 2;
	rect.y = 2;
	rect.w = TILE_COLS * 2;
	rect.h = TILE_ROWS * 2;
	SDL_FillRect(surface, &rect, 0);
	rect.x = 2;
	rect.y = 2;
	rect.w = 2;
	rect.h = 2;
	fillColor = 0xff0000;
	maxx = TILE_COLS * 2 + 2;

	for(int i = 0; i < TILE_COUNT; i++) {
		ifs.read(l, 1);
		if(l[0] != -1) {
			SDL_FillRect(surface, &rect, fillColor);
		}
		rect.x += 2;
		if(rect.x >= maxx) {
			rect.x = 2;
			rect.y += 2;
		}
	}

	ifs.close();

	return surface;
}

int Level::tile(int x, int y) {
	// Calculate which tile intersects with (x, y)
	return ((int)(y / TILE_H) * TILE_COLS) + (int)(x / TILE_W);
}

void Level::draw(SDL_Surface * screen) {
	SDL_Rect rect;
	SDL_Rect rect_s;

	rect.w = TILE_W;
	rect.h = TILE_H;

	if(background == NULL)
		SDL_FillRect(screen, &screen->clip_rect, 0);
	else
		SDL_BlitSurface(background, NULL, screen, NULL);

	// Draw each sprite, one by one
	for(int i = 0; i < TILE_COUNT; i++) {
		// Don't draw empty sprites
		if(level[i] == -1) continue;

		rect.x = (i % TILE_COLS) * TILE_W;
		rect.y = (i / TILE_COLS) * TILE_H;

		rect_s.x = level[i] * TILE_W;
		rect_s.y = 0;
		rect_s.w = TILE_W;
		rect_s.h = TILE_H;

		// Show damaged tiles
		if(level_hp[i] < 40) {
			rect_s.y += TILE_H;
		}
		if(level_hp[i] < 20) {
			rect_s.y += TILE_H;
		}

		// Show bouncing tiles
		if(level_bounce[i] != 0) {
			level_bounce[i]++;
			level_bounce_start[i] = Gameplay::frame;
			if(level_bounce[i] >= BOUNCE_LAST_FRAME) {
				level_bounce[i] = 0;
			}
			if(level_bounce[i] <= BOUNCE_TOP_FRAME) {
				rect.y -= level_bounce[i] * BOUNCE_HEIGHT_PER_FRAME;
			} else {
				rect.y -= (BOUNCE_TOP_FRAME - (level_bounce[i] - BOUNCE_TOP_FRAME)) * BOUNCE_HEIGHT_PER_FRAME;
			}
		}

		SDL_BlitSurface(tiles, &rect_s, screen, &rect);
	}
}

void Level::reset() {
	for(int i = 0; i < TILE_COUNT; i++) {
		level[i] = level_start[i];
		level_hp[i] = level_hp_start[i];
		level_bounce[i] = 0;
		level_bounce_start[i] = 0;
	}
}


bool Level::is_intersecting(SDL_Rect * rect) {
	// Check if the rect is colliding with the level
	int l, r, t, b;

	l = rect->x;
	r = rect->x + rect->w - 1;

	t = rect->y;
	b = rect->y + rect->h - 1;

	if(t < 0) t = 0;
	if(b < 0) b = 0;
	if(t >= WINDOW_HEIGHT) t = WINDOW_HEIGHT - 1;
	if(b >= WINDOW_HEIGHT) b = WINDOW_HEIGHT - 1;

	if(l < 0) {
		for(int x = l + WINDOW_WIDTH; x < WINDOW_WIDTH; x++) {
			if(level[tile(x, t)] != -1)
				return true;
			if(level[tile(x, b)] != -1)
				return true;
		}

		for(int y = t; y < b; y++) {
			if(level[tile(l + WINDOW_WIDTH, y)] != -1)
				return true;
			if(level[tile(l + WINDOW_WIDTH, y)] != -1)
				return true;
		}

		l = 0;
	}

	if(r >= WINDOW_WIDTH) {
		for(int x = 0; x < r - WINDOW_WIDTH; x++) {
			if(level[tile(x, t)] != -1)
				return true;
			if(level[tile(x, b)] != -1)
				return true;
		}

		for(int y = t; y < b; y++) {
			if(level[tile(r - WINDOW_WIDTH, y)] != -1)
				return true;
			if(level[tile(r - WINDOW_WIDTH, y)] != -1)
				return true;
		}
		
		r = WINDOW_WIDTH - 1;
	}

	for(int x = l; x < r; x++) {
		if(r >= WINDOW_WIDTH)
			break;

		if(level[tile(x, t)] != -1)
			return true;
		if(level[tile(x, b)] != -1)
			return true;
	}

	if(l >= WINDOW_WIDTH) l -= WINDOW_WIDTH - 1;

	for(int y = t; y < b; y++) {
		if(level[tile(l, y)] != -1)
			return true;
		if(level[tile(r, y)] != -1)
			return true;
	}

	return false;
}

bool Level::is_on_bottom(SDL_Rect * rect) {
	// Check if there is anything to stand on below the rect
	int l, r, t, b;

	l = rect->x;
	r = rect->x + rect->w - 1;

	t = rect->y;
	b = rect->y + rect->h - 1;

	// Above the screen is no bottom
	if(b < 0) return false;

	if(l >= WINDOW_WIDTH) l -= WINDOW_WIDTH;
	if(r >= WINDOW_WIDTH) r -= WINDOW_WIDTH;

	if(level[tile(l, b + 1)] != -1)
		return true;
	if(level[tile(r, b + 1)] != -1)
		return true;
	return false;
}

void Level::damage_tiles(SDL_Rect * rect, int damage) {
	int l, r, t, b;

	l = rect->x;
	r = rect->x + rect->w + TILE_W;

	t = rect->y;
	b = rect->y + rect->h;

	if(t < 0) t = 0;
	if(b < 0) b = 0;
	if(t >= WINDOW_HEIGHT) t = WINDOW_HEIGHT - 1;
	if(b >= WINDOW_HEIGHT) b = WINDOW_HEIGHT - 1;

	int wx;

	for(int y = t; y < b; y += TILE_H) {
		if(y < 0) continue;
		if(y > WINDOW_HEIGHT) continue;

		for(int x = l; x < r; x += TILE_W) {
			wx = x;
			if(x < 0) wx += WINDOW_WIDTH;
			if(x > WINDOW_WIDTH) wx -= WINDOW_WIDTH;

			if(level[tile(wx, y)] != -1) {
				level_hp[tile(wx, y)] -= damage;
				if(level_hp[tile(wx, y)] < 0)
					level[tile(wx, y)] = -1;
			}
		}
	}
}

void Level::bounce_tile(SDL_Rect * rect) {
	int l, x, r, y;
	int pos;

	l = rect->x;
	r = rect->x + rect->w;
	x = rect->x + (rect->w / 2);
	y = rect->y;

	if(x >= WINDOW_WIDTH) x = x - WINDOW_WIDTH;
	if(r >= WINDOW_WIDTH) r = r - WINDOW_WIDTH;
	
	pos = tile(x, y); // Is there a tile at the center-top?
	if(level[pos] == -1) pos = tile(l, y); // tile at left-top?
	if(level[pos] == -1) pos = tile(r, y); // tile at right-top?
	if(level[pos] == -1) return; // No tile at all

	// Cancel if the tile is already bouncing
	if(level_bounce[pos] != 0) return;

	// Start bouncing
	level_bounce[pos] = 1;
	level_bounce_start[pos] = Gameplay::frame;

	// Check if a player is standing on the tile
	SDL_Rect rect_hit;
	rect_hit.x = (pos % TILE_COLS) * TILE_W;
	rect_hit.y = ((pos / TILE_COLS) * TILE_H) - BOUNCE_HIT_HEIGHT;
	rect_hit.w = TILE_W;
	rect_hit.h = BOUNCE_HIT_HEIGHT;

	Gameplay::instance->bounce_up_players_and_npcs(&rect_hit);
}

