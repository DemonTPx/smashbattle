#ifndef _LOCALMULTIPLAYERROUNDEND_H
#define _LOCALMULTIPLAYERROUNDEND_H

#define ROUNDEND_CHANGE_CHARACTER 1
#define ROUNDEND_CHANGE_LEVEL 2
#define ROUNDEND_QUIT 0

class LocalMultiplayerRoundEnd {
public:
	LocalMultiplayerRoundEnd(int players);

	void run();

	int result;
	int winner;
	int round;

	Player ** player;
private:
	void init();
	void cleanup();

	void draw();

	void handle_input(SDL_Event * event);
	void process_cursor();

	void select_up();
	void select_down();
	void select();

	static const char * item[];
	static const int ITEMCOUNT;

	std::vector<SDL_Surface*> * surf_items;
	std::vector<SDL_Rect*> * surf_items_clip;

	int players;

	bool ready;

	int frame;
	
	ControlScheme controls1, controls2;
	ControlScheme controls3, controls4;

	int selected_item;

	int cursor_direction;
	bool cursor_first;
	int cursor_direction_start;
	bool cursor_enter;
};

#endif
