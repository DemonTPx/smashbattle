#ifndef _BOMBPOWERUP_H
#define _BOMBPOWERUP_H

class BombPowerUp : public PowerUp {
public:
	BombPowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position, int ammount);
	~BombPowerUp();

	void got_powerup(Player *);
	void show(SDL_Surface *);
	
	SDL_Surface * surface;
	SDL_Rect * clip;
	int ammount;
};

#endif
