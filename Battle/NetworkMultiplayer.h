#pragma once

#include "LocalMultiplayer.h"

class NetworkMultiplayer : public LocalMultiplayer 
{
public:
	NetworkMultiplayer (Main &main) : LocalMultiplayer(main) {}
	~NetworkMultiplayer() {}

protected:

	virtual void on_game_reset();

	virtual void on_pre_processing() {}
	virtual void on_post_processing();

	virtual GameplayObject *generate_powerup(bool force = false);
};