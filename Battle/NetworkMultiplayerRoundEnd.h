#pragma once

#include "SDL/SDL.h"

#include "LocalMultiplayerRoundEnd.h"

class NetworkMultiplayerRoundEnd : public LocalMultiplayerRoundEnd {
public:
	NetworkMultiplayerRoundEnd(Main &main, Uint32 displayMilliseconds);
	~NetworkMultiplayerRoundEnd();

protected:

	virtual void do_run();

	virtual void reset_input() {}

	virtual void process_cursor() {}

	virtual void handle_event(GameInput &input, SDL_Event &event) {};

	virtual void draw_menu() {}
        
        virtual int get_player_number(Player &player);

private:

	Uint32 displayStartTime_;
	Uint32 displayMilliseconds_;

};
