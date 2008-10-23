#ifndef _PLAYER_H
#define _PLAYER_H

#define FACE_LEFT 0
#define FACE_RIGHT 1

#define SPR_R		0
#define SPR_R_WALK1	1
#define SPR_R_WALK2	2
#define SPR_R_WALK3	3
#define SPR_R_BRAKE	4
#define SPR_R_JUMP	5
#define SPR_L		6
#define SPR_L_WALK1	7
#define SPR_L_WALK2	8
#define SPR_L_WALK3	9
#define SPR_L_BRAKE	10
#define SPR_L_JUMP	11

#define CYCLE_UP 0
#define CYCLE_DN 1

class Player {
public:
	Player(const char * sprite_file);
	~Player();
	SDL_Rect * position;
	SDL_Surface * sprites;
	SDL_Rect * clip[20];

	int momentumx, momentumy;
	int last_speedx, last_speedy;
	bool keydn_l, keydn_r, keydn_u;
	bool is_jumping, is_falling;
	int jump_start;
	int current_sprite;
	int cycle_direction;
	int distance_walked;

	int key_l, key_r, key_u;

	static const int jump_height;

	void handle_input(SDL_Event * event);
	void show(SDL_Surface * screen);
	void set_sprite(int sprite);
	void cycle_sprite(int first, int last);
	void cycle_sprite_updown(int first, int last);

private:
	void load_images(const char * sprite_file);
	void free_images();
	void set_clips();
};

#endif