#ifndef _HEALTHPOWERUP_H
#define _HEALTHPOWERUP_H

class HealthPowerUp : public PowerUp {
public:
	HealthPowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position, int hp);
	void cleanup();

	void got_powerup(Player *);
	void show(SDL_Surface *);
	
	SDL_Surface * surface;
	SDL_Rect * clip;
	int hp;
};

#endif
