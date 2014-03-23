#pragma once

#define ROUNDEND_CHANGE_CHARACTER 1
#define ROUNDEND_CHANGE_LEVEL 2
#define ROUNDEND_QUIT 0

#include "SimpleDrawable.h"

class LocalMultiplayerRoundEnd : public SimpleDrawable {
public:
	LocalMultiplayerRoundEnd(Main &main);
	~LocalMultiplayerRoundEnd();

	void run();

	int result;
	Player * winner;
	int round;

	void add_player(Player * p);

protected:

	virtual void draw_impl();

private:
	void init();
	void cleanup();

	void process_cursor();

	void select_up();
	void select_down();
	void select();

	static const char * item[];
	static const int ITEMCOUNT;

	std::vector<SDL_Surface*> * surf_items;
	std::vector<SDL_Rect*> * surf_items_clip;

	SDL_Surface * surf_result;

	SDL_Surface * background;
	
	std::vector<Player*> * players;

	short order[4];

	bool ready;

	int frame;
	
	GameInput * input;

	int selected_item;

	Main &main_;
};
