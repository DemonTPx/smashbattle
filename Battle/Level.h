#ifndef _LEVEL_H
#define _LEVEL_H

#include "SDL/SDL_mixer.h"
#include <vector>

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

#define LEVEL_ID 0x5342 // "SB"
#define LEVEL_VERSION 2

#define LEVEL_BLOCK_PSTART				0x01
#define LEVEL_BLOCK_PROP				0x02
#define LEVEL_BLOCK_MISSION				0x10
#define LEVEL_BLOCK_POWERUP				0x20
#define LEVEL_BLOCK_POWERUP_DISPENSER	0x21
#define LEVEL_BLOCK_NPC					0x30
#define LEVEL_BLOCK_NPC_DISPENSER		0x31

struct LEVEL_POINT {
	short x;
	short y;
};

struct LEVEL_RECT {
	short x;
	short y;
	short w;
	short h;
};

struct LEVEL_HEADER {
	unsigned short id;
	unsigned short version;
};

struct LEVEL_META {
	char name[20];
	char author[20];
	bool multiplayer;
	unsigned short max_players;
	unsigned int background_color;
	char filename_tiles[30];
	char filename_background[30];
	char filename_props[30];
	char filename_music[30];
};

struct LEVEL_TILE {
	unsigned short tile;
	unsigned short hp;
	bool indestructible;
	bool bouncing;
	bool show_in_preview;
};

struct LEVEL_PLAYERSTART {
	unsigned short player;
	short x;
	short y;
	bool facing_right;
};

struct LEVEL_PROP {
	LEVEL_RECT src;
	LEVEL_POINT dst;
};

enum {
	LM_TYPE_NONE = 0,
	LM_TYPE_KILL_ALL
};

struct LEVEL_MISSION {
	int character;
	int type;
	int bullets;
	int doubledamagebullets;
	int instantkillbullets;
	int bombs;
	int kill_all_time_gold;
	int kill_all_time_silver;
};

enum {
	L_PU_HEALTH,
	L_PU_AMMO,
	L_PU_DOUBLEDAMAGE,
	L_PU_INSTANTKILL,
	L_PU_BOMB,
	L_PU_AIRSTRIKE,
	L_PU_LASERBEAM
};

struct LEVEL_POWERUP {
	int type;
	LEVEL_POINT position;
};

struct LEVEL_POWERUP_DISPENSER {
	bool global;
	LEVEL_POINT position; // (ignored on global)
	int rate; // chance per frame; 60 is ~1 per second
	int max; // max powerups on screen (global only)
	int rate_health;
	int rate_bullet;
	int rate_doubledamage;
	int rate_instantkill;
	int rate_bomb;
	int rate_airstrike;
	int rate_laserbeam;
};

enum {
	L_NPC_CHICK,
	L_NPC_CANNON,
	L_NPC_GATLING
};

struct LEVEL_NPC {
	int type;
	LEVEL_POINT position;
	int move_direction;
};

struct LEVEL_NPC_DISPENSER {
	int type; // NPC type
	LEVEL_POINT position;
	int rate; // chance per frame; 60 is ~1 per second
	int max; // maximum NPCs to dispence
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
	static LEVEL_META * get_meta(const char * filename);
	static LEVEL_MISSION * get_mission(const char * filename);

	static SDL_Surface * get_thumbnail(const char * filename);
	static SDL_Surface * get_preview(const char * filename);

	static int tile_pos(int x, int y);

	LEVEL_TILE tile[TILE_COUNT];
	LEVEL_PLAYERSTART playerstart[4];

	LEVEL_MISSION mission;

	int level[TILE_COUNT];
	int level_hp[TILE_COUNT];

	int level_bounce[TILE_COUNT];
	int level_bounce_start[TILE_COUNT];

	Mix_Music * music;

	std::vector<LEVEL_POWERUP *> * powerups;
	std::vector<LEVEL_POWERUP_DISPENSER *> * powerup_dispensers;

	std::vector<LEVEL_NPC *> * npcs;
	std::vector<LEVEL_NPC_DISPENSER *> * npc_dispensers;
private:
	SDL_Surface * tiles;
	SDL_Surface * background;
};

#endif
