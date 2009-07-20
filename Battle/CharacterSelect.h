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

	void process_cursors();

	void select(int * select, int direction);

	int players;

	bool ready;
	bool player_ready[4];

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
