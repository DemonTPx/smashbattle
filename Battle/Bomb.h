#ifndef _BOMB_H
#define _BOMB_H

#include "GameplayObject.h"
#include "Player.h"

#define BOMB_W 12
#define BOMB_H 16

class Bomb : public GameplayObject {
public:
	Bomb();
	Bomb(SDL_Surface * surface);
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

	void explode();

	virtual void draw(SDL_Surface * screen);

	SDL_Rect * get_damage_rect();
private:
	void set_clips();
};

#endif
