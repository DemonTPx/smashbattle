#ifndef _CHARACTERSELECT_H
#define _CHARACTERSELECT_H

#include "PlayerAnimation.h"

class CharacterSelect {
public:
	CharacterSelect(int players);
	void run();

	char * name[4];
	char * file[4];

	int player_select[4];

	bool cancel;
private:
	void draw();

	void handle_input(SDL_Event * event);

	void init();
	void clean_up();

	void prerender_background();

	void process_cursors();

	void process_random_players();

	void select(int player);

	SDL_Surface * background;

	int players;

	bool ready;
	bool player_ready[4];

	bool player_random[4];
	int player_random_start[4];
	int player_random_before[4];

	PlayerAnimation * playeranimation[4];
	
	int cursor_direction[4];
	bool cursor_first[4];
	int cursor_direction_start[4];
	bool cursor_enter[4];
	
	ControlScheme controls[4];

	bool flicker[4];
	int flicker_start[4];
	int flicker_frame[4];

	int frame;
};

#endif
