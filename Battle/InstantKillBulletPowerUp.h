#ifndef _INSTANTKILLBULLETPOWERUP_H
#define _INSTANTKILLBULLETPOWERUP_H

class InstantKillBulletPowerUp : public PowerUp {
public:
	InstantKillBulletPowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position, int ammo);
	void cleanup();

	void got_powerup(Player *);
	void show(SDL_Surface *);
	
	SDL_Surface * surface;
	SDL_Rect * clip;
	int ammo;
};

#endif
