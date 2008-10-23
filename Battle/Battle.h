#ifndef _BATTLE_H
#define _BATTLE_H

#include "Player.h"
class Battle {
public:
	Battle();
	~Battle();
	void run();
	static const int level[];
private:
	bool game_running;
	void draw_level(SDL_Surface * screen);
	void draw_pause_screen(SDL_Surface * screen);
	void move_player(Player * p);
	void check_player_collision(Player * p1, Player * p2);
	bool check_collision(SDL_Rect * rect);
	bool check_bottom(SDL_Rect * rect);
	int level_pos(int x, int y);
	
	void load_images();
	void free_images();
	void set_clips();

	bool paused;
	
	TTF_Font * font;
	SDL_Color fontColor;

	SDL_Surface * tiles;
	SDL_Surface * background;
	SDL_Rect * tile_rect[8];
};

#endif