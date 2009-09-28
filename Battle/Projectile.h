#ifndef _PROJECTILE_H
#define _PROJECTILE_H

#include "GameplayObject.h"

class Projectile : public GameplayObject {
public:
	Projectile();
	Projectile(SDL_Surface * surface, SDL_Rect * clip);
	~Projectile();

	SDL_Surface * sprite;
	SDL_Rect * clip;

	int speedx, speedy;
	int distance_traveled;
	int max_distance;
	bool hit;
	
	Player * owner;

	int damage;
	
	void show(SDL_Surface * screen);
	
	virtual void move(Level * level);
	virtual void process();

	virtual void hit_player(Player * player);
	virtual void hit_npc(NPC * npc);

	virtual void draw(SDL_Surface * screen);
private:
};

#endif
