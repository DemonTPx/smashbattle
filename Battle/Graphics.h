#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include "SDL/SDL.h"

#include <vector>

#define SPR_R		0
#define SPR_R_WALK1	1
#define SPR_R_WALK2	2
#define SPR_R_WALK3	3
#define SPR_R_RUN1	4
#define SPR_R_RUN2	5
#define SPR_R_RUN3	6
#define SPR_L_BRAKE	7
#define SPR_R_JUMP	8
#define SPR_R_DUCK	9
#define SPR_L		10
#define SPR_L_WALK1	11
#define SPR_L_WALK2	12
#define SPR_L_WALK3	13
#define SPR_L_RUN1	14
#define SPR_L_RUN2	15
#define SPR_L_RUN3	16
#define SPR_R_BRAKE	17
#define SPR_L_JUMP	18
#define SPR_L_DUCK	19

#define SPR_COUNT	22

#define SPR_AVATAR	20
#define SPR_AVATAR_SELECTED 21

#define PLAYER_SURF_COLS 10
#define PLAYER_SURF_COUNT 20

#define CUP_W 30
#define CUP_H 28

class Graphics {
public:
	Graphics();
	~Graphics();
	
	void load_all();
	void clear_all();

	void load_players();
	void clear_players();
	//void create_player_masks();
	void set_player_clips();
	void clear_player_clips();

	static Uint32 combine_colors(Uint32 color1, Uint32 color2);
	static SDL_Surface * load_icon(const char * filename, Uint8 ** mask, Uint32 color);

	SDL_Surface * weapons;
	SDL_Surface * bombs;
	SDL_Surface * powerups;

	SDL_Surface * shield;
	
	SDL_Surface * player1hp;
	SDL_Surface * player2hp;
	
	SDL_Surface * common;
	SDL_Surface * pmarker;

	SDL_Surface * bg_grey;
	SDL_Surface * bg_menu;

	SDL_Surface * cups;

	SDL_Surface * tiles;

	SDL_Surface * npc_chick;
	SDL_Surface * npc_cannon;
	SDL_Surface * npc_gatling;

	SDL_Surface * statsblock[3];

	SDL_Surface * text_ready;
	SDL_Surface * text_random;
	
	SDL_Surface * text_pressstart;

	std::vector<SDL_Surface*> * player;
	std::vector<SDL_Surface*> * playername;

	SDL_Rect * player_clip[SPR_COUNT];
	SDL_Rect * pmarker_clip_above[4];
	SDL_Rect * pmarker_clip_below[4];
private:
	SDL_Surface * load_bmp(const char * filename);
};

#endif
