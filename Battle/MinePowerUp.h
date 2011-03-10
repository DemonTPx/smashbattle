#ifndef _MINEPOWERUP_H
#define _MINEPOWERUP_H

class MinePowerUp : public GameplayObject {
public:
	MinePowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position, int ammount);
	~MinePowerUp();

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
