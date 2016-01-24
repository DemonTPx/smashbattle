
#include "SimpleDrawable.h"

#pragma once

class LevelSelect : public SimpleDrawable {
public:
	LevelSelect(Main &main);
	void run();

	int level;
	bool cancel;

protected:

	virtual void draw_impl();

private:

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
	
	GameInput * input;
	
	int flicker, flicker_frame;

	std::vector<SDL_Surface*> * thumbs;
	std::vector<SDL_Surface*> * backgrounds;

	int frame;

	Main &main_;
};
