#ifndef _RANDOMPOWERUP_H
#define _RANDOMPOWERUP_H

class RandomPowerUp : public GameplayObject {
public:
	RandomPowerUp(SDL_Surface * surface, SDL_Rect * position);
	~RandomPowerUp();

	static const int CYCLE_COUNT;
	static const int CYCLE_X[];
	static const int CYCLE_DELAY;

	virtual void move(Level * level);
	virtual void process();

	virtual void hit_player(Player * player);
	virtual void hit_npc(NPC * npc);

	virtual void draw(SDL_Surface * screen, int frames_processed = 0);

	SDL_Surface * surface;
	SDL_Rect * clip;

	int cycle;
	
	int frame_last_cycle;
	int frame_cycle;
};

#endif
