#pragma once

class HealthPowerUp : public GameplayObject {
public:
	HealthPowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position, int hp, Main &main);
	~HealthPowerUp();

	virtual void move(Level * level);
	virtual void process();

	virtual void hit_player(Player * player);
	virtual void hit_npc(NPC * npc);

	virtual void draw(SDL_Surface * screen, int frames_processed = 0);

	virtual void copyTo(network::CommandGeneratePowerup &powerup);
	
	SDL_Surface * surface;
	SDL_Rect * clip;
	int hp;

	Main &main_;
};
