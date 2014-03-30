#pragma once

#define ROUNDEND_CHANGE_CHARACTER 1
#define ROUNDEND_CHANGE_LEVEL 2
#define ROUNDEND_QUIT 0

#include <vector>

#include "SimpleDrawable.h"

class Player;
class SDL_Surface;
class SDL_Rect;
union SDL_Event;
class GameInput;
class Main;

class LocalMultiplayerRoundEnd : public SimpleDrawable {
public:
	LocalMultiplayerRoundEnd(Main &main);
	virtual ~LocalMultiplayerRoundEnd();

	void run();

	int result;
	Player * winner;
	int round;

	void add_player(Player * p);

protected:

	virtual void draw_impl();

	virtual void do_run() {};

	virtual void reset_input();

	virtual void process_cursor();

	virtual void handle_event(GameInput &input, SDL_Event &event);

	virtual void draw_menu();

	bool ready;

private:
	void init();
	void cleanup();

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

	int frame;
	
	GameInput * input;

	int selected_item;

	Main &main_;
};
