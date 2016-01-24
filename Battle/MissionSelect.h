#pragma once

#include "SimpleDrawable.h"

class MissionSelect : public SimpleDrawable {
public:
	MissionSelect(Main &main);
	void run();

	bool cancel;

	int mission;

protected:

	virtual void draw_impl();

private:

	void load_sprites();
	void free_sprites();

	void process_cursor();

	void select(int direction);
	
	bool ready;
	bool ready_mission;

	bool cancel_selected;

	int mission_scroll_top;
	
	GameInput * input;
	
	int flicker, flicker_frame;

	SDL_Surface * background;

	std::vector<SDL_Surface*> * thumbs;

	int frame;

	Main &main_;
};
