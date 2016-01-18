#pragma once

#include "GameplayObject.h"
#include "KillMove.h"
#include "Player.h"

#define BOMB_W 12
#define BOMB_H 16

class Bomb : public GameplayObject {
public:
	Bomb(Main &main);
	Bomb(SDL_Surface * surface, Main &main);
	~Bomb();

	SDL_Surface * sprite;
	SDL_Rect * clip[5];
	int explosion_offset_x, explosion_offset_y;

	Player * owner;

	int speedy;

	bool exploded;

	int damage;
	int time;

	bool hit_on_impact;

	KillMove kill_move;

	int current_frame;
	int frame_start;
	int frame_change_start;
	int frame_change_count;
	int frame_explode;
	int flicker_frame;

	static const int FRAME_COUNT;
	static const int FRAME_NORMAL;
	static const int FRAME_FLASH;
	static const int FRAME_STRIKE_NORMAL;
	static const int FRAME_STRIKE_FLASH;
	static const int FRAME_EXPLOSION;
	
	virtual void move(Level * level);
	virtual void process();

	virtual void hit_player(Player * player);
	virtual void hit_npc(NPC * npc);

	virtual void explode();

	SDL_Rect * get_damage_rect();
protected:

	virtual void draw_impl(SDL_Surface * screen, int frames_processed = 0);

	void set_clips();

	Main &main_;
};
