#ifndef _HEALTHPOWERUP_H
#define _HEALTHPOWERUP_H

class HealthPowerUp : public GameplayObject {
public:
	HealthPowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position, int hp);
	~HealthPowerUp();

	virtual void move(Level * level);
	virtual void process();

	virtual void hit_player(Player * player);
	virtual void hit_npc(NPC * npc);

	virtual void draw(SDL_Surface * screen);
	
	SDL_Surface * surface;
	SDL_Rect * clip;
	int hp;
};

#endif
