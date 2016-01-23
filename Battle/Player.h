#pragma once

#include "Level.h"
#include "GameInput.h"
#include "KillMove.h"

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

struct Kill {
	Player * victim;
	Player * killer;
	KillMove move;
};

namespace network {
class CommandSetPlayerData;
}

class Player;
namespace player_util
{
	void set_position_data(network::CommandSetPlayerData &data, char client_id, Uint32 time, short udpseq, Player &player);
	void set_player_data(Player &player, network::CommandSetPlayerData &data, bool skip_input = false);
	Player &get_player_by_id(Main &main, char client_id);
	void unset_input(Player &player);
}

class Projectile;
class Bomb;

class Player {
public:
	Player(int character, int number,  Main &main);
	~Player();

	static const int CHARACTER_COUNT;
	static const Character CHARACTERS[];
	static const int COLORS_COUNT;
	static const int COLORS[];

	static const short SUIT_COLOR_COUNT;
	static const Uint32 SUIT_ORIGINAL[];
	static const Uint32 SUIT_REPLACE[4][5];
	
	static const int SPEEDCLASS_COUNT;
	static const SpeedClass SPEEDCLASSES[];
	static const int WEIGHTCLASS_COUNT;
	static const WeightClass WEIGHTCLASSES[];
	static const int WEAPONCLASS_COUNT;
	static const WeaponClass WEAPONCLASSES[];
	static const int BOMBPOWERCLASS_COUNT;
	static const BombPowerClass BOMBPOWERCLASSES[];

	void set_character(int character);
	void set_sprites();
	void reset(bool excludeInputs = false, bool excludeStats = false);
	void update_suit();

	void spectate(bool set = true);
	bool spectating();

	char * name;
	int character;
	int number;
	int suit_number;

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
	int mines;
	int doubledamagebullets;
	int instantkillbullets;

	int score;

	int hitpoints;

	int bounce_direction_x, bounce_direction_y;

	Player * last_damage_player = NULL;
	KillMove last_damage_move = UNKNOWN;
	Player * last_pushed_player = NULL;

	std::vector<Kill> * kill_list = 0;

	int rounds_won;
	unsigned int bullets_fired;
	unsigned int bullets_hit;
	unsigned int bombs_fired;
	unsigned int bombs_hit;
	unsigned int headstomps;

	unsigned int kills;
	unsigned int deaths;

	static const int jump_height;

	bool is_spectating;

	void draw(SDL_Surface * screen, bool marker = false, int frames_processed = 0);

	void move(Level * level);
	void process();
	Projectile * create_projectile_for_player(Sint16 x, Sint16 y);
	Projectile * create_projectile(Sint16 x, Sint16 y);
	Bomb * create_bomb_for_player(Sint16 x, Sint16 y);
	Bomb * create_bomb(Sint16 x, Sint16 y);

	void bounce(Player * other);
	void bounce_up(SDL_Rect * source);

	bool damage(int damage, Player * other, KillMove move);
	bool damage(int damage, Player * other, KillMove move, bool force);

	void set_sprite(int sprite);
	void cycle_sprite(int first, int last);
	void cycle_sprite_updown(int first, int last);
	SDL_Rect * get_rect();

	Main &main_;
private:
};
