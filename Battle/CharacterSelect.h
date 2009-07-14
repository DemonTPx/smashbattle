#ifndef _CHARACTERSELECT_H
#define _CHARACTERSELECT_H

class CharacterSelect {
public:
	CharacterSelect(int players);
	void run();

	char * name[4];
	char * file[4];

	int player_select[4];

	bool cancel;
private:
	void draw();

	void load_sprites();
	void free_sprites();

	void handle_input(SDL_Event * event);

	void process_cursors();

	void select(int * select, int direction);

	int players;

	bool ready;
	bool player_ready[4];
	
	int cursor_direction[4];
	bool cursor_first[4];
	int cursor_direction_start[4];
	bool cursor_enter[4];
	
	ControlScheme controls[4];

	std::vector<SDL_Surface*> * character_sprites;
	SDL_Rect * clip_avatar, * clip_avatar_selected, * clip_left, * clip_right;

	bool flicker[4];
	int flicker_start[4];
	int flicker_frame[4];

	int frame;
};

#endif
