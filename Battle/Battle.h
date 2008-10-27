#ifndef _BATTLE_H
#define _BATTLE_H

#include "Player.h"
#include "Projectile.h"
#include <vector>

class Battle {
public:
	Battle();
	~Battle();
	void run();
	static const int level[];
private:
	bool game_running;

	void reset_game();
	
	void draw_level(SDL_Surface * screen);
	void draw_pause_screen(SDL_Surface * screen);
	void draw_score(SDL_Surface * screen);

	void handle_draw_countdown(SDL_Surface * screen);
	
	void move_player(Player * p);
	void move_projectile(Projectile * p);
	
	void process_shoot(Player * p);

	void check_player_collision(Player * p1, Player * p2);
	void check_player_projectile_collision(Player * p);
	bool check_collision(SDL_Rect * rect);
	bool check_bottom(SDL_Rect * rect);

	int level_pos(int x, int y);
	
	void load_images();
	void free_images();
	void set_clips();

	Player * player1;
	Player * player2;

	bool paused;
	bool countdown;
	int countdown_sec_left;
	Timer * countdown_timer;

	int frame;

	std::vector<Projectile*> * projectiles;
	
	TTF_Font * font26;
	TTF_Font * font52;
	SDL_Color fontColor;

	SDL_Surface * tiles;
	SDL_Surface * background;
	SDL_Rect * tile_rect[8];
};

#endif
