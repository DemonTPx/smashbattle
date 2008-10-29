#ifndef _CHARACTERSELECT_H
#define _CHARACTERSELECT_H

class CharacterSelect {
public:
	CharacterSelect();
	void run();

	char * name1,* name2;
	char * file1,* file2;
	
	bool ready1, ready2;
private:
	void draw();

	void load_fonts();
	void free_fonts();

	TTF_Font * font26;
	TTF_Font * font13;
	SDL_Color fontColor;
};

#endif
