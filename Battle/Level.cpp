#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"
#include "zlib.h"

#include <iostream>
#include <fstream>

#include "Main.h"
#include "Gameplay.h"
#include "Level.h"

const int Level::LEVEL_COUNT = 16;
const LevelInfo Level::LEVELS[Level::LEVEL_COUNT] = {
	{(char*)"TRAINING DOJO", (char*)"stage/trainingdojo.lvl"},
	{(char*)"PLATFORM ALLEY", (char*)"stage/platformalley.lvl"},
	{(char*)"PITTFALL", (char*)"stage/pitfall.lvl"},
	{(char*)"DUCK'N'HUNT", (char*)"stage/ducknhunt.lvl"},
	{(char*)"COMMON GROUNDS", (char*)"stage/commongrounds.lvl"},
	{(char*)"POGOSTICK", (char*)"stage/pogostick.lvl"},
	{(char*)"LA MOUSTACHE", (char*)"stage/lamoustache.lvl"},
	{(char*)"THE FUNNEL", (char*)"stage/thefunnel.lvl"},
	{(char*)"BLAST BOWL", (char*)"stage/blastbowl.lvl"},
	{(char*)"PIT OF DEATH", (char*)"stage/pitofdeath.lvl"},
	{(char*)"RABBIT HOLE", (char*)"stage/rabbithole.lvl"},
	{(char*)"STAY HIGH", (char*)"stage/stayhigh.lvl"},
	{(char*)"PIE PIT", (char*)"stage/piepit.lvl"},
	{(char*)"SLIP'N'SLIDE", (char*)"stage/slipnslide.lvl"},
	{(char*)"BOULDERDASH", (char*)"stage/boulderdash.lvl"},
	{(char*)"SNOW FIGHT", (char*)"stage/snowfight.lvl"}
};

Level::Level() {
	background = NULL;
	tiles = NULL;

	music = NULL;
	
	memset(&mission, 0, sizeof(LEVEL_MISSION));
	mission.type = LM_TYPE_NONE;
	
	powerups = new std::vector<LEVEL_POWERUP*>(0);
	powerup_dispensers = new std::vector<LEVEL_POWERUP_DISPENSER*>(0);
	npcs = new std::vector<LEVEL_NPC*>(0);
	npc_dispensers = new std::vector<LEVEL_NPC_DISPENSER*>(0);
}

Level::~Level() {
	if(background != NULL)
		SDL_FreeSurface(background);
	if(tiles != NULL)
		SDL_FreeSurface(tiles);

	if(music != NULL)
		Mix_FreeMusic(music);
	
	for(unsigned int i = 0; i < powerups->size(); i++) {
		delete powerups->at(i);
	}
	delete powerups;
	
	for(unsigned int i = 0; i < powerup_dispensers->size(); i++) {
		delete powerup_dispensers->at(i);
	}
	delete powerup_dispensers;

	for(unsigned int i = 0; i < npcs->size(); i++) {
		delete npcs->at(i);
	}
	delete npcs;

	for(unsigned int i = 0; i < npc_dispensers->size(); i++) {
		delete npc_dispensers->at(i);
	}
	delete npc_dispensers;
}

void Level::load(const char * filename) {
	gzFile file;
	
	LEVEL_HEADER header;
	LEVEL_META meta;
	LEVEL_PLAYERSTART pstart;
	LEVEL_PROP * prop;
	LEVEL_POWERUP * powerup;
	LEVEL_POWERUP_DISPENSER * powerup_dispenser;
	LEVEL_NPC * npc;
	LEVEL_NPC_DISPENSER * npc_dispenser;

	std::vector<LEVEL_PROP *> * props;
	unsigned short block_id;

	char tiles_file_full[35], bg_file_full[35], props_file_full[35];
	char music_file_full[45];

	SDL_Surface * loaded, * surface;
	SDL_Rect rect, rect_s;
	Uint32 colorkey;

	file = gzopen(filename, "rb");
	gzread(file, &header, sizeof(LEVEL_HEADER));
	
	if(header.id != LEVEL_ID) // Invalid file
		return;
	if(header.version != LEVEL_VERSION) // Invalid version
		return;

	gzread(file, &meta, sizeof(LEVEL_META));

	gzread(file, &tile, sizeof(tile));

	props = new std::vector<LEVEL_PROP *>(0);
	while(!gzeof(file)) {
		gzread(file, &block_id, sizeof(block_id));
		switch(block_id) {
			// Player start
			case LEVEL_BLOCK_PSTART:
				gzread(file, &pstart, sizeof(LEVEL_PLAYERSTART));
				if(pstart.player == 0xffff) break;
				memcpy(&playerstart[pstart.player], &pstart, sizeof(LEVEL_PLAYERSTART));
				break;
			// Prop
			case LEVEL_BLOCK_PROP:
				prop = new LEVEL_PROP();
				gzread(file, prop, sizeof(LEVEL_PROP));
				props->push_back(prop);
				break;
			// Mission
			case LEVEL_BLOCK_MISSION:
				gzread(file, &mission, sizeof(LEVEL_MISSION));
				break;
			// Powerup
			case LEVEL_BLOCK_POWERUP:
				powerup = new LEVEL_POWERUP();
				gzread(file, powerup, sizeof(LEVEL_POWERUP));
				powerups->push_back(powerup);
				break;
			// Powerup dispenser
			case LEVEL_BLOCK_POWERUP_DISPENSER:
				powerup_dispenser = new LEVEL_POWERUP_DISPENSER();
				gzread(file, powerup_dispenser, sizeof(LEVEL_POWERUP_DISPENSER));
				powerup_dispensers->push_back(powerup_dispenser);
				break;
			// NPC
			case LEVEL_BLOCK_NPC:
				npc = new LEVEL_NPC();
				gzread(file, npc, sizeof(LEVEL_NPC));
				npcs->push_back(npc);
				break;
			// NPC dispenser
			case LEVEL_BLOCK_NPC_DISPENSER:
				npc_dispenser = new LEVEL_NPC_DISPENSER();
				gzread(file, npc_dispenser, sizeof(LEVEL_NPC_DISPENSER));
				npc_dispensers->push_back(npc_dispenser);
				break;
		}
	}

	gzclose(file);

	// LOAD TILES SURFACE

	strncpy(tiles_file_full, "gfx/\0", 5);
	strncat(tiles_file_full, meta.filename_tiles, 30);

	surface = SDL_LoadBMP(tiles_file_full);
	tiles = SDL_DisplayFormat(surface);
	colorkey = SDL_MapRGB(tiles->format, 0, 255, 255);
	SDL_SetColorKey(tiles, SDL_SRCCOLORKEY, colorkey);
	SDL_FreeSurface(surface);

	// PRE-RENDER THE BACKGROUND

	background = SDL_CreateRGBSurface(0, WINDOW_WIDTH, WINDOW_HEIGHT, 32, 0, 0, 0, 0);
	SDL_FillRect(background, 0, meta.background_color);

	// Draw the background image (tiled) into the background
	if(meta.filename_background[0] != 0) {
		strncpy(bg_file_full, "gfx/\0", 5);
		strncat(bg_file_full, meta.filename_background, 30);

		loaded = SDL_LoadBMP(bg_file_full);
		surface = SDL_DisplayFormat(loaded);
		SDL_FreeSurface(loaded);

		for(int y = 0; y < WINDOW_HEIGHT; y += surface->h) {
			for(int x = 0; x < WINDOW_WIDTH; x += surface->w) {
				rect.x = x;
				rect.y = y;
				rect.w = surface->w;
				rect.h = surface->h;
				SDL_BlitSurface(surface, 0, background, &rect);
			}
		}

		SDL_FreeSurface(surface);
	}

	// Draw the props into the background
	if(meta.filename_props[0] != 0) {
		strncpy(props_file_full, "gfx/\0", 5);
		strncat(props_file_full, meta.filename_props, 30);

		loaded = SDL_LoadBMP(props_file_full);
		surface = SDL_DisplayFormat(loaded);
		colorkey = SDL_MapRGB(surface->format, 0, 255, 255);
		SDL_SetColorKey(surface, SDL_SRCCOLORKEY, colorkey);
		SDL_FreeSurface(loaded);

		for(unsigned int i = 0; i < props->size(); i++) {
			prop = props->at(i);
			rect_s.x = prop->src.x;
			rect_s.y = prop->src.y;
			rect_s.w = prop->src.w;
			rect_s.h = prop->src.h;
			rect.x = prop->dst.x;
			rect.y = prop->dst.y;
			SDL_BlitSurface(surface, &rect_s, background, &rect);
			delete prop;
		}

		delete props;
		SDL_FreeSurface(surface);
	}

	// Draw the static tiles into the background (static = non-bouncing indestructible)
	if(meta.filename_props[0] != 0) {
		for(int i = 0; i < TILE_COUNT; i++) {
			if(tile[i].tile == 0xffff)
				continue;
			if(tile[i].bouncing)
				continue;
			if(!tile[i].indestructible)
				continue;

			rect_s.x = (TILE_W * tile[i].tile);
			rect_s.y = 0;
			rect_s.w = TILE_W;
			rect_s.h = TILE_H;
			
			if(tile[i].hp < 40) {
				rect_s.y += TILE_H;
			}
			if(tile[i].hp < 20) {
				rect_s.y += TILE_H;
			}

			rect.x = (i % TILE_COLS) * TILE_W;
			rect.y = (i / TILE_COLS) * TILE_H;

			SDL_BlitSurface(tiles, &rect_s, background, &rect);
		}
	}
	
	// Load music file
	strncpy(music_file_full, "music/\0", 7);
	strncat(music_file_full, meta.filename_music, 30);

	music = Mix_LoadMUS(music_file_full);
}

LEVEL_HEADER * Level::get_header(const char *filename) {
	gzFile file;
	LEVEL_HEADER * header;
	
	header = new LEVEL_HEADER();

	file = gzopen(filename, "rb");
	gzread(file, header, sizeof(LEVEL_HEADER));
	
	if(header->id != LEVEL_ID) // Invalid file
		return NULL;
	if(header->version != LEVEL_VERSION) // Invalid version
		return NULL;

	gzclose(file);

	return header;
}

LEVEL_META * Level::get_meta(const char * filename) {
	gzFile file;
	LEVEL_HEADER header;
	LEVEL_META * meta;

	file = gzopen(filename, "rb");
	gzread(file, &header, sizeof(LEVEL_HEADER));
	
	if(header.id != LEVEL_ID) // Invalid file
		return NULL;
	if(header.version != LEVEL_VERSION) // Invalid version
		return NULL;

	meta = new LEVEL_META();

	gzread(file, meta, sizeof(LEVEL_META));

	gzclose(file);

	return meta;
}

LEVEL_MISSION * Level::get_mission(const char * filename) {
	gzFile file;
	LEVEL_HEADER header;
	LEVEL_MISSION * mission;
	unsigned short block_id;

	mission = new LEVEL_MISSION();
	memset(mission, 0, sizeof(LEVEL_MISSION));
	mission->type = LM_TYPE_NONE;

	file = gzopen(filename, "rb");
	gzread(file, &header, sizeof(LEVEL_HEADER));
	
	if(header.id != LEVEL_ID) // Invalid file
		return NULL;
	if(header.version != LEVEL_VERSION) // Invalid version
		return NULL;

	gzseek(file, sizeof(LEVEL_META), SEEK_CUR);
	while(gzeof(file)) {
		gzread(file, &block_id, sizeof(block_id));
		switch(block_id) {
			case LEVEL_BLOCK_PSTART:
				gzseek(file, sizeof(LEVEL_PLAYERSTART), SEEK_CUR);
				break;
			case LEVEL_BLOCK_PROP:
				gzseek(file, sizeof(LEVEL_PROP), SEEK_CUR);
				break;
			case LEVEL_BLOCK_MISSION:
				gzread(file, mission, sizeof(LEVEL_MISSION));
				break;
			case LEVEL_BLOCK_POWERUP:
				gzseek(file, sizeof(LEVEL_POWERUP), SEEK_CUR);
				break;
			case LEVEL_BLOCK_POWERUP_DISPENSER:
				gzseek(file, sizeof(LEVEL_POWERUP_DISPENSER), SEEK_CUR);
				break;
			case LEVEL_BLOCK_NPC:
				gzseek(file, sizeof(LEVEL_NPC), SEEK_CUR);
				break;
			case LEVEL_BLOCK_NPC_DISPENSER:
				gzseek(file, sizeof(LEVEL_NPC_DISPENSER), SEEK_CUR);
				break;
		}
	}

	gzclose(file);

	return mission;
}

SDL_Surface * Level::get_thumbnail(const char * filename) {
	gzFile file;
	LEVEL_HEADER header;
	LEVEL_META meta;
	LEVEL_TILE tile[TILE_COUNT];

	SDL_Rect rect;
	Uint32 fillColor;
	int maxx;

	SDL_Surface * surface;

	file = gzopen(filename, "rb");
	gzread(file, &header, sizeof(LEVEL_HEADER));
	
	if(header.id != LEVEL_ID) // Invalid file
		return NULL;
	if(header.version != LEVEL_VERSION) // Invalid version
		return NULL;
	
	gzread(file, &meta, sizeof(LEVEL_META));

	gzread(file, &tile, sizeof(tile));
	gzclose(file);

	surface = SDL_CreateRGBSurface(NULL, TILE_COLS * 2 + 4, TILE_ROWS * 2 + 4, 32, 0, 0, 0, 0);
	SDL_FillRect(surface, NULL, 0x444444);
	rect.x = 2;
	rect.y = 2;
	rect.w = TILE_COLS * 2;
	rect.h = TILE_ROWS * 2;
	SDL_FillRect(surface, &rect, 0);
	rect.x = 2;
	rect.y = 2;
	rect.w = 2;
	rect.h = 2;
	fillColor = 0x993300;
	maxx = TILE_COLS * 2 + 2;

	for(int i = 0; i < TILE_COUNT; i++) {
		if(tile[i].tile != 0xffff) {
			SDL_FillRect(surface, &rect, fillColor);
		}
		rect.x += 2;
		if(rect.x >= maxx) {
			rect.x = 2;
			rect.y += 2;
		}
	}

	return surface;
}

SDL_Surface * Level::get_preview(const char * filename) {
	gzFile file;
	LEVEL_HEADER header;
	LEVEL_META meta;
	LEVEL_TILE tile[TILE_COUNT];
	LEVEL_PLAYERSTART pstart;
	LEVEL_PROP * prop;
	std::vector<LEVEL_PROP *> * props;
	unsigned short block_id;

	SDL_Rect rect, rect_s;
	Uint32 colorkey;

	char tiles_file_full[35], bg_file_full[35], props_file_full[35];

	SDL_Surface * loaded;
	SDL_Surface * surface_s;
	SDL_Surface * surface;

	file = gzopen(filename, "rb");
	gzread(file, &header, sizeof(LEVEL_HEADER));
	
	if(header.id != LEVEL_ID) // Invalid file
		return NULL;
	if(header.version != LEVEL_VERSION) // Invalid version
		return NULL;

	gzread(file, &meta, sizeof(LEVEL_META));

	gzread(file, &tile, sizeof(tile));
	
	props = new std::vector<LEVEL_PROP *>(0);
	while(!gzeof(file)) {
		gzread(file, &block_id, sizeof(block_id));
		switch(block_id) {
			case LEVEL_BLOCK_PSTART:
				gzread(file, &pstart, sizeof(LEVEL_PLAYERSTART));
				break;
			case LEVEL_BLOCK_PROP:
				prop = new LEVEL_PROP();
				gzread(file, prop, sizeof(LEVEL_PROP));
				props->push_back(prop);
				break;
		}
	}

	gzclose(file);

	// PRE-RENDER THE BACKGROUND

	surface = SDL_CreateRGBSurface(0, WINDOW_WIDTH, WINDOW_HEIGHT, 32, 0, 0, 0, 0);
	SDL_FillRect(surface, 0, meta.background_color);

	// Draw the background image (tiled) into the background
	if(meta.filename_background[0] != 0) {
		strncpy(bg_file_full, "gfx/\0", 5);
		strncat(bg_file_full, meta.filename_background, 30);

		loaded = SDL_LoadBMP(bg_file_full);
		surface_s = SDL_DisplayFormat(loaded);
		SDL_FreeSurface(loaded);

		for(int y = 0; y < WINDOW_HEIGHT; y += surface_s->h) {
			for(int x = 0; x < WINDOW_WIDTH; x += surface_s->w) {
				rect.x = x;
				rect.y = y;
				rect.w = surface_s->w;
				rect.h = surface_s->h;
				SDL_BlitSurface(surface_s, 0, surface, &rect);
			}
		}

		SDL_FreeSurface(surface_s);
	}

	// Draw the props into the background
	if(meta.filename_props[0] != 0) {
		strncpy(props_file_full, "gfx/\0", 5);
		strncat(props_file_full, meta.filename_props, 30);

		loaded = SDL_LoadBMP(props_file_full);
		surface_s = SDL_DisplayFormat(loaded);
		colorkey = SDL_MapRGB(surface_s->format, 0, 255, 255);
		SDL_SetColorKey(surface_s, SDL_SRCCOLORKEY, colorkey);
		SDL_FreeSurface(loaded);

		for(unsigned int i = 0; i < props->size(); i++) {
			prop = props->at(i);
			rect_s.x = prop->src.x;
			rect_s.y = prop->src.y;
			rect_s.w = prop->src.w;
			rect_s.h = prop->src.h;
			rect.x = prop->dst.x;
			rect.y = prop->dst.y;
			SDL_BlitSurface(surface_s, &rect_s, surface, &rect);
			delete prop;
		}

		delete props;
		SDL_FreeSurface(surface_s);
	}

	// Draw the preview tiles into the background
	if(meta.filename_tiles[0] != 0) {
		strncpy(tiles_file_full, "gfx/\0", 5);
		strncat(tiles_file_full, meta.filename_tiles, 30);

		loaded = SDL_LoadBMP(tiles_file_full);
		surface_s = SDL_DisplayFormat(loaded);
		colorkey = SDL_MapRGB(surface_s->format, 0, 255, 255);
		SDL_SetColorKey(surface_s, SDL_SRCCOLORKEY, colorkey);
		SDL_FreeSurface(loaded);

		for(int i = 0; i < TILE_COUNT; i++) {
			if(tile[i].tile == 0xffff)
				continue;
			if(!tile[i].show_in_preview)
				continue;

			rect_s.x = (TILE_W * tile[i].tile);
			rect_s.y = 0;
			rect_s.w = TILE_W;
			rect_s.h = TILE_H;

			if(tile[i].hp < 40)
				rect_s.y += TILE_H;
			if(tile[i].hp < 20)
				rect_s.y += TILE_H;

			rect.x = (i % TILE_COLS) * TILE_W;
			rect.y = (i / TILE_COLS) * TILE_H;

			SDL_BlitSurface(surface_s, &rect_s, surface, &rect);
		}

		SDL_FreeSurface(surface_s);
	}

	return surface;
}

int Level::tile_pos(int x, int y) {
	// Calculate which tile intersects with (x, y)
	return ((int)(y / TILE_H) * TILE_COLS) + (int)(x / TILE_W);
}

void Level::draw(SDL_Surface * screen) {
	SDL_Rect rect;
	SDL_Rect rect_s;

	rect.w = TILE_W;
	rect.h = TILE_H;

	SDL_BlitSurface(background, NULL, screen, NULL);

	// Draw each sprite, one by one

	for(int i = 0; i < TILE_COUNT; i++) {
		// Don't draw empty sprites
		if(level[i] == -1) continue;

		// Skip tile if it is prerendered
		if(!tile[i].bouncing && tile[i].indestructible) continue;

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
		if(tile[i].tile == 0xffff)
			level[i] = -1;
		else
			level[i] = (int)tile[i].tile;

		if(tile[i].indestructible)
			level_hp[i] = 0x7fffffff;
		else
			level_hp[i] = (int)tile[i].hp;

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
			if(level[tile_pos(x, t)] != -1)
				return true;
			if(level[tile_pos(x, b)] != -1)
				return true;
		}

		for(int y = t; y < b; y++) {
			if(level[tile_pos(l + WINDOW_WIDTH, y)] != -1)
				return true;
			if(level[tile_pos(l + WINDOW_WIDTH, y)] != -1)
				return true;
		}

		l = 0;
	}

	if(r >= WINDOW_WIDTH) {
		for(int x = 0; x < r - WINDOW_WIDTH; x++) {
			if(level[tile_pos(x, t)] != -1)
				return true;
			if(level[tile_pos(x, b)] != -1)
				return true;
		}

		for(int y = t; y < b; y++) {
			if(level[tile_pos(r - WINDOW_WIDTH, y)] != -1)
				return true;
			if(level[tile_pos(r - WINDOW_WIDTH, y)] != -1)
				return true;
		}
		
		r = WINDOW_WIDTH - 1;
	}

	for(int x = l; x < r; x++) {
		if(r >= WINDOW_WIDTH)
			break;

		if(level[tile_pos(x, t)] != -1)
			return true;
		if(level[tile_pos(x, b)] != -1)
			return true;
	}

	if(l >= WINDOW_WIDTH) l -= WINDOW_WIDTH - 1;

	for(int y = t; y < b; y++) {
		if(level[tile_pos(l, y)] != -1)
			return true;
		if(level[tile_pos(r, y)] != -1)
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

	if(level[tile_pos(l, b + 1)] != -1)
		return true;
	if(level[tile_pos(r, b + 1)] != -1)
		return true;
	return false;
}

void Level::damage_tiles(SDL_Rect * rect, int damage) {
	int l, r, t, b;

	l = rect->x;
	r = rect->x + rect->w - 1;

	t = rect->y;
	b = rect->y + rect->h;

	// Align rect to the tiles
	l = l - (l % TILE_W);
	r = r - (r % TILE_W) + TILE_W;

	t = t - (t % TILE_H);
	b = b - (t % TILE_H);

	if(t < 0) t = 0;
	if(b < 0) b = 0;
	if(t >= WINDOW_HEIGHT) t = WINDOW_HEIGHT - 1;
	if(b >= WINDOW_HEIGHT) b = WINDOW_HEIGHT - 1;

	int wx, pos;

	for(int y = t; y < b; y += TILE_H) {
		if(y < 0) continue;
		if(y > WINDOW_HEIGHT) continue;

		for(int x = l; x < r; x += TILE_W) {
			wx = x;
			if(x < 0) wx += WINDOW_WIDTH;
			if(x > WINDOW_WIDTH) wx -= WINDOW_WIDTH;

			pos = tile_pos(wx, y);

			if(!tile[pos].indestructible) {
				level_hp[pos] -= damage;
				if(level_hp[pos] <= 0)
					level[pos] = -1;
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
	
	pos = tile_pos(x, y); // Is there a tile at the center-top?
	if(level[pos] == -1) pos = tile_pos(l, y); // tile at left-top?
	if(level[pos] == -1) pos = tile_pos(r, y); // tile at right-top?
	if(level[pos] == -1) return; // No tile at all

	if(!tile[pos].bouncing) return; // Tile is not bouncable

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

	Gameplay::instance->bounce_up_players_and_npcs(&rect_hit, rect);
}

