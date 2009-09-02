#ifndef _LOCALMULTIPLAYERROUNDEND_H
#define _LOCALMULTIPLAYERROUNDEND_H

#define ROUNDEND_CHANGE_CHARACTER 1
#define ROUNDEND_CHANGE_LEVEL 2
#define ROUNDEND_QUIT 0

class LocalMultiplayerRoundEnd {
public:
	LocalMultiplayerRoundEnd();
	~LocalMultiplayerRoundEnd();

	void run();

	int result;
	Player * winner;
	int round;

	void add_player(Player * p);
private:
	void init();
	void cleanup();

	void draw();

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
};

#endif
