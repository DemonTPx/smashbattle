#ifndef _MISSIONSELECT_H
#define _MISSIONSELECT_H

class MissionSelect {
public:
	MissionSelect();
	void run();

	bool cancel;

	int mission;
private:
	void draw();

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
};

#endif
