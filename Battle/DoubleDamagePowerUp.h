#ifndef _DOUBLEDAMAGEPOWERUP_H
#define _DOUBLEDAMAGEPOWERUP_H

class DoubleDamagePowerUp : public PowerUp {
public:
	DoubleDamagePowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position, int ammo);
	void cleanup();

	void got_powerup(Player *);
	void show(SDL_Surface *);
	
	SDL_Surface * surface;
	SDL_Rect * clip;
	int ammo;
};

#endif
