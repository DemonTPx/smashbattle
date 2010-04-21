#ifndef _PLAYER_H
#define _PLAYER_H

#include "Main.h"
#include "Level.h"

#define FACE_LEFT 0
#define FACE_RIGHT 1

#define CYCLE_UP 0
#define CYCLE_DN 1

#define PLAYER_W 22
#define PLAYER_H 44
#define PLAYER_DUCK_H 26

#define DUCK_FORCE_FRAMES 10

struct Character {
	char * name;
	char * filename;
	int speedclass;
	int weightclass;
	int weaponclass;
	int bombpowerclass;
};

struct SpeedClass {
	int run_speed;
};

struct WeightClass {
	int push_force;
	int headjump_damage;
	int bounce_momentum_x;
	int bounce_momentum_y;
};

struct WeaponClass {
	int rate;
	int distance;
	int damage;
};

struct BombPowerClass {
	int damage;
};

class Player {
public:
	Player(int character, int number);
	~Player();

	static const int CHARACTER_COUNT;
	static const Character CHARACTERS[];
	static const int COLORS[];
	
	static const int SPEEDCLASS_COUNT;
	static const SpeedClass SPEEDCLASSES[];
	static const int WEIGHTCLASS_COUNT;
	static const WeightClass WEIGHTCLASSES[];
	static const int WEAPONCLASS_COUNT;
	static const WeaponClass WEAPONCLASSES[];
	static const int BOMBPOWERCLASS_COUNT;
	static const BombPowerClass BOMBPOWERCLASSES[];

	void set_character(int character);
	void reset();

	char * name;
	int character;
	int number;

	SDL_Rect * position;
	SDL_Rect * last_position;
	SDL_Surface * sprites;
	SDL_Rect * marker_clip_above;
	SDL_Rect * marker_clip_below;

	int speedclass;
	int weightclass;
	int weaponclass;
	int bombpowerclass;

	int momentumx, momentumy;
	
	int newmomentumx;

	bool is_running;
	bool is_duck;
	bool is_duck_forced;
	int duck_force_start;

	bool is_jumping, is_falling;
	int jump_start;

	int current_sprite;
	int cycle_direction;
	int distance_walked;

	GameInput * input;

	bool is_hit;
	int hit_start;
	int hit_delay;
	int hit_flicker_frame;

	bool is_frozen;
	int freeze_start;

	bool is_dead;
	int dead_start;

	bool is_shielded;
	int shield_start;
	int shield_frame;

	int shoot_start;
	int shoot_delay;

	int bomb_start;
	int bomb_delay;

	int bullets;
	int bombs;
	int doubledamagebullets;
	int instantkillbullets;

	int score;

	int hitpoints;

	int bounce_direction_x, bounce_direction_y;

	int rounds_won;
	unsigned int bullets_fired;
	unsigned int bullets_hit;
	unsigned int bombs_fired;
	unsigned int bombs_hit;
	unsigned int headstomps;

	static const int jump_height;

	void draw(SDL_Surface * screen, bool marker = false);

	void move(Level * level);
	void process();

	void bounce(Player * other);
	void bounce_up(SDL_Rect * source);

	bool damage(int damage);

	void set_sprite(int sprite);
	void cycle_sprite(int first, int last);
	void cycle_sprite_updown(int first, int last);
	SDL_Rect * get_rect();
private:
};

#endif
