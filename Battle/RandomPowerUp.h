#pragma once

namespace network {
class CommandGeneratePowerup;
}

class RandomPowerUp : public GameplayObject {
public:
	RandomPowerUp(SDL_Surface * surface, SDL_Rect * position, Main &main);
	~RandomPowerUp();

	static const int CYCLE_COUNT;
	static const int CYCLE_X[];
	static const int CYCLE_DELAY;

	virtual void move(Level * level);
	virtual void process();

	virtual void hit_player(Player * player);
	virtual void hit_npc(NPC * npc);

	virtual void copyTo(network::CommandGeneratePowerup &powerup);

	SDL_Surface * surface;
	SDL_Rect * clip;

	int cycle;
	
	int frame_cycle;

	int frame_counter;

	Main &main_;

protected:

	virtual void draw_impl(SDL_Surface * screen, int frames_processed = 0);

};
