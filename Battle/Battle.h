#ifndef _BATTLE_H
#define _BATTLE_H

#include "Player.h"
#include "Projectile.h"
#include "Bomb.h"
#include "PowerUp.h"
#include <vector>

#define BULLETS_UNLIMITED -1

struct Character {
	char * name;
	char * filename;
};

struct Stage {
	char * name;
	char * author;
	char * filename;
};

struct RuleSet {
	char * name;
	int bullets;
	int doubledamagebullets;
	int instantkillbullets;
	int bombs;
	int healthpowerups;
	int bulletpowerups;
	int doubledamagepowerups;
	int instantkillpowerups;
	int bombpowerups;
	int powerup_rate;
	int powerup_max;
};

class Battle {
public:
	Battle();
	~Battle();
	void run();

	static const int CHARACTER_COUNT;
	static const Character characters[];
	static const int STAGE_COUNT;
	static const Stage stages[];
	static const int RULESET_COUNT;
	static const RuleSet rulesets[];
	static SDL_Surface * create_level_thumbnail(const char * filename);

	SDL_Surface * tiles;
	SDL_Surface * background;
	SDL_Surface * weapons;
	SDL_Surface * surface_bombs;
	SDL_Surface * powerup;
	SDL_Surface * player1hp;
	SDL_Surface * player2hp;
	SDL_Rect * tile_rect[16];
private:
	bool game_running;

	void load_level(const char * filename);

	void reset_game();
	
	void draw_level(SDL_Surface * screen);
	void draw_pause_screen(SDL_Surface * screen);
	void draw_score(SDL_Surface * screen);
	void draw_win_screen(SDL_Surface * screen);

	void handle_pause_input(SDL_Event * event);

	void handle_draw_countdown(SDL_Surface * screen);
	
	void move_player(Player * p);
	void move_projectile(Projectile * p);
	void move_bomb(Bomb * b);
	
	void process_shoot(Player * p);

	void generate_powerup(bool force);

	void check_player_collision(Player * p1, Player * p2);
	void check_player_projectile_collision(Player * p);
	void check_player_bomb_collision(Player * p);
	void check_player_powerup_collision(Player * p);
	bool check_collision(SDL_Rect * rect);
	bool check_bottom(SDL_Rect * rect);

	bool is_intersecting(SDL_Rect * one, SDL_Rect * two);

	int level_pos(int x, int y);
	
	void load_images();
	void free_images();
	void set_clips();
	
	int level[300];

	Player * player1;
	Player * player2;

	bool paused;
	Player * pause_player;
	int pause_quit;
	
	bool ended;
	Timer * end_timer;
	int end_avatar_start_frame;

	bool countdown;
	int countdown_sec_left;
	Timer * countdown_timer;

	int frame;

	std::vector<Projectile*> * projectiles;
	std::vector<Bomb*> * bombs;
	std::vector<PowerUp*> * powerups;

	RuleSet ruleset;
	
	TTF_Font * font26;
	TTF_Font * font52;
	SDL_Color fontColor;
};

#endif
