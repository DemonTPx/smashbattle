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

	void select(int direction);

	int players;
	
	bool ready;
	bool ready_level;
	
	int cursor_direction[4];
	bool cursor_first[4];
	int cursor_direction_start[4];
	bool cursor_enter[4];
	
	ControlScheme controls[4];
	
	int flicker, flicker_frame;

	char * name;
	std::vector<SDL_Surface*> * thumbs;

	int frame;
};

#endif
