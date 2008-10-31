#ifndef _AMMOPOWERUP_H
#define _AMMOPOWERUP_H

class AmmoPowerUp : public PowerUp {
public:
	AmmoPowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position, int ammo);
	~AmmoPowerUp();

	void got_powerup(Player *);
	void show(SDL_Surface *);
	
	SDL_Surface * surface;
	SDL_Rect * clip;
	int ammo;
};

#endif
