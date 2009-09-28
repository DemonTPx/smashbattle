#ifndef _AMMOPOWERUP_H
#define _AMMOPOWERUP_H

class AmmoPowerUp : public GameplayObject {
public:
	AmmoPowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position, int ammo);
	~AmmoPowerUp();

	virtual void move(Level * level);
	virtual void process();

	virtual void hit_player(Player * player);
	virtual void hit_npc(NPC * npc);

	virtual void draw(SDL_Surface * screen);
	
	SDL_Surface * surface;
	SDL_Rect * clip;
	int ammo;
};

#endif
