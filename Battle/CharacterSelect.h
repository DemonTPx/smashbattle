#ifndef _CHARACTERSELECT_H
#define _CHARACTERSELECT_H

class CharacterSelect {
public:
	CharacterSelect();
	void run();

	char * name1,* name2;
	char * file1,* file2;

	int stage;
private:
	void draw();

	void load_fonts();
	void free_fonts();

	void load_sprites();
	void free_sprites();

	void select(int * select, int direction);

	void select_stage(int direction);

	bool ready;	
	bool ready1, ready2;
	int select1, select2;

	std::vector<SDL_Surface*> * character_sprites;
	SDL_Rect * clip_avatar, * clip_avatar_selected, * clip_left, * clip_right;

	bool flicker1, flicker2;
	int flicker1_start, flicker2_start;
	int flicker1_frame, flicker2_frame;
	char * stage_name;
	char * stage_author;
	std::vector<SDL_Surface*> * stage_thumbs;

	int frame;

	TTF_Font * font26;
	TTF_Font * font13;
	SDL_Color fontColor;
};

#endif
