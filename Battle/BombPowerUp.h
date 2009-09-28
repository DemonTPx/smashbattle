#ifndef _BOMBPOWERUP_H
#define _BOMBPOWERUP_H

class BombPowerUp : public GameplayObject {
public:
	BombPowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position, int ammount);
	~BombPowerUp();

	virtual void move(Level * level);
	virtual void process();

	virtual void hit_player(Player * player);
	virtual void hit_npc(NPC * npc);

	virtual void draw(SDL_Surface * screen);
	
	SDL_Surface * surface;
	SDL_Rect * clip;
	int ammount;
};

#endif
