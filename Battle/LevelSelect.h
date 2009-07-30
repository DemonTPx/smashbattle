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

	void process_cursors();

	void process_random();

	void select(int direction);

	int players;
	
	bool ready;
	bool ready_level;

	bool random;
	int random_start;
	int random_before;
	
	GameInput * input[4];
	
	int flicker, flicker_frame;

	std::vector<SDL_Surface*> * thumbs;
	std::vector<SDL_Surface*> * backgrounds;

	int frame;
};

#endif
