#pragma once

#include "LocalMultiplayer.h"

class LagMeasure;

namespace network{

class ClientNetworkMultiplayer : public LocalMultiplayer 
{
public:
	ClientNetworkMultiplayer(Main &main);
	~ClientNetworkMultiplayer() {}

	/**
	 * Before we can run() the game, we need stuff like level, players, etc.,
	 *  until that stuff is available, start() handles the mainloop.
	 */
	void start();

	/**
	 * Render logged text to output "CONSOLE" to screen.
	 */
	void draw_console();

protected:

	virtual void on_game_reset();

	virtual void on_pre_processing() {}
	virtual void on_post_processing() {}

	virtual void on_input_handled();

	virtual GameplayObject *generate_powerup(bool force = false);

private:
	LagMeasure *lag_;

	Main &main_;

};
}