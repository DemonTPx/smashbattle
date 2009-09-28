#ifndef _INSTANTKILLBULLETPOWERUP_H
#define _INSTANTKILLBULLETPOWERUP_H

class InstantKillBulletPowerUp : public GameplayObject {
public:
	InstantKillBulletPowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position, int ammo);
	~InstantKillBulletPowerUp();

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
