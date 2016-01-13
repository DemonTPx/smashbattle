#pragma once

#include "GameplayObject.h"
#include "KillMove.h"

class Projectile : public GameplayObject {
public:
	Projectile(Main &main);
	Projectile(SDL_Surface * surface, SDL_Rect * clip, Main &main);
	~Projectile();

	SDL_Surface * sprite;
	SDL_Rect * clip;

	int speedx, speedy;
	int distance_traveled;
	int max_distance;
	bool hit;

	KillMove kill_move;
	
	Player * owner;

	int damage;
	
	void show(SDL_Surface * screen);
	
	virtual void move(Level * level);
	virtual void process();

	virtual void hit_player(Player * player);
	virtual void hit_npc(NPC * npc);

protected:

	virtual void draw_impl(SDL_Surface * screen, int frames_processed = 0);

private:

	Main &main_;
};
