#pragma once

namespace network {
class CommandGeneratePowerup;
}

class ShieldPowerUp : public GameplayObject {
public:
	ShieldPowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position, Main &main);
	~ShieldPowerUp();

	virtual void move(Level * level);
	virtual void process();

	virtual void hit_player(Player * player);
	virtual void hit_npc(NPC * npc);

	virtual void copyTo(network::CommandGeneratePowerup &powerup);
	
	SDL_Surface * surface;
	SDL_Rect * clip;

	Main &main_;

protected:

	virtual void draw_impl(SDL_Surface * screen, int frames_processed = 0);
};
