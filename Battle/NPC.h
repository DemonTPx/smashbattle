#ifndef _NPC_H
#define _NPC_H

#include "Level.h"
#include "Player.h"

#define CYCLE_UP 0
#define CYCLE_DN 1

class NPC {
public:
	NPC();
	~NPC();

	SDL_Rect * position;
	SDL_Rect * last_position;
	SDL_Surface * sprites;
	
	void draw(SDL_Surface * screen);

	virtual void move(Level * level);
	virtual void process();
	
	virtual void reset();

	virtual void hit_player_side(Player * p);
	virtual void hit_player_top_bottom(Player * p);
	
	void bounce(Player * other);
	void bounce(NPC * other);
	void bounce_up(SDL_Rect * source);

	bool damage(int damage);
	
	void set_sprite(int sprite);
	void cycle_sprite(int first, int last);
	void cycle_sprite_updown(int first, int last);

	SDL_Rect * get_rect();

	int momentumx;
	int momentumy;
	
	int newmomentumx;

	bool done;

	bool is_stationary;

	bool is_jumping;
	bool is_falling;

	bool is_hit;
	int hit_start;
	int hit_delay;
	int hit_flicker_frame;

	bool is_frozen;
	int freeze_start;
	int freeze_delay;

	bool is_dead;
	int dead_start;

	int hitpoints;

	int move_direction;
protected:
	int frame_w;
	int frame_h;

	int frames;

	int frame_idle;

	int frame_walk_first;
	int frame_walk_last;

	int frame_jump_first;
	int frame_jump_last;

	int frame_brake_first;
	int frame_brake_last;

	int frame_dead;

	int max_speed;
	int jump_height;

	int bounce_weight;

	int current_sprite;
	int cycle_direction;
	int distance_walked;
	int jump_cycle_start;

	int bounce_direction_x, bounce_direction_y;
};

#endif
