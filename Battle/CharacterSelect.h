#pragma once

#include "PlayerAnimation.h"
#include "SimpleDrawable.h"

class CharacterSelect : public SimpleDrawable {
public:
	CharacterSelect(Main &main);

	CharacterSelect(int max_possible_players, int required_num_players, Main &main);
	
	void run();

	char * name[4];
	char * file[4];

	bool player_joined[4];
	int player_select[4];

	int player_select_first = 0;

	bool cancel;
protected:

	virtual void draw_impl();

private:

	void init();
	void clean_up();

	void prerender_background();

	void process_cursors();

	void process_random_players();

	void select(int player, int direction);

	SDL_Surface * background;

	bool ready;
	bool player_ready[4];

	bool player_random[4];
	int player_random_start[4];
	int player_random_before[4];

	PlayerAnimation * playeranimation[4];

	GameInput * input[4];

	bool flicker[4];
	int flicker_start[4];
	int flicker_frame[4];

	int frame;
	int possible_players_;
	int required_num_players_;

	Main &main_;
};
