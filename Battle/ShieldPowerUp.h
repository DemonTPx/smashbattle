#ifndef _SHIELDPOWERUP_H
#define _SHIELDPOWERUP_H

class ShieldPowerUp : public GameplayObject {
public:
	ShieldPowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position);
	~ShieldPowerUp();

	virtual void move(Level * level);
	virtual void process();

	virtual void hit_player(Player * player);
	virtual void hit_npc(NPC * npc);

	virtual void draw(SDL_Surface * screen);
	
	SDL_Surface * surface;
	SDL_Rect * clip;
};

#endif
