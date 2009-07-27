#ifndef _LEVELSELECT_H
#define _LEVELSELECT_H

class LevelSelect {
public:
	LevelSelect(int players);
	void run();

	int level;
	bool cancel;
private:
	void draw();

	void load_sprites();
	void free_sprites();

	void handle_input(SDL_Event * event);

	void process_cursors();

	void process_random();

	void select(int direction);

	int players;
	
	bool ready;
	bool ready_level;

	bool random;
	int random_start;
	int random_before;
	
	int cursor_direction[4];
	bool cursor_first[4];
	int cursor_direction_start[4];
	bool cursor_enter[4];
	
	ControlScheme controls[4];
	
	int flicker, flicker_frame;

	std::vector<SDL_Surface*> * thumbs;
	std::vector<SDL_Surface*> * backgrounds;

	int frame;
};

#endif
