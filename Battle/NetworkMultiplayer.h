#pragma once

#include "LocalMultiplayer.h"

class NetworkMultiplayer : public LocalMultiplayer 
{
public:
	NetworkMultiplayer (Main &main);
	~NetworkMultiplayer() {};

	enum class State {
		DISPLAYING_DEATH,
		DISPLAYING_WINNER,
		DISPLAYING_COUNTDOWN,
		DISPLAYING_SCREEN,
		DONE
	};
	
protected:

	virtual void on_game_reset();

	virtual void on_pre_processing() {}
	virtual void on_post_processing();

	virtual void game_interrupted_reset() { round = 1; }

	virtual GameplayObject *generate_powerup(bool force = false);

	State currentState_;
	Uint32 currentStateBeginTime_;
	Uint32 currentStateBeginTimeDelay_;

private:
	
	void process_keep_alives();

};
