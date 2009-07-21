#ifndef _LEVEL_H
#define _LEVEL_H

// Tile sizes, dimensions and count
#define TILE_W 32
#define TILE_H 32
#define TILE_COLS 20
#define TILE_ROWS 15
#define TILE_COUNT (TILE_COLS * TILE_ROWS)

// Tile boucing variables
#define BOUNCE_TOP_FRAME 4
#define BOUNCE_LAST_FRAME 8
#define BOUNCE_HEIGHT_PER_FRAME 3
#define BOUNCE_HIT_HEIGHT 16

struct LevelInfo {
	char * name;
	char * filename;
};

#define LEVEL_ID 'SB'
#define LEVEL_VERSION 1

struct LEVEL_TILE {
	unsigned short tile;
	unsigned short hp;
	bool indestructible;
	bool bouncing;
	bool show_in_preview;
	unsigned short player_start;
};

struct LEVEL_HEADER {
	unsigned short id;
	unsigned short version;
	char name[20];
	char author[20];
	unsigned short max_players;
	char filename_tiles[30];
	char filename_background[30];
};

class Level {
public:
	Level();
	~Level();

	void draw(SDL_Surface * screen);

	void load(const char * filename);

	void reset();

	void damage_tiles(SDL_Rect * rect, int damage);
	void bounce_tile(SDL_Rect * rect);

	bool is_intersecting(SDL_Rect * rect);
	bool is_on_bottom(SDL_Rect * rect);

	static const int LEVEL_COUNT;
	static const LevelInfo LEVELS[];

	static LEVEL_HEADER * get_header(const char * filename);
	static SDL_Surface * get_thumbnail(const char * filename);
	static SDL_Surface * get_preview(const char * filename);

	static int tile(int x, int y);
	
	int level_start[TILE_COUNT];
	int level_hp_start[TILE_COUNT];

	int level[TILE_COUNT];
	int level_hp[TILE_COUNT];

	int level_bounce[TILE_COUNT];
	int level_bounce_start[TILE_COUNT];
private:
	SDL_Surface * tiles;
	SDL_Surface * background;
};

#endif
