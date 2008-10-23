#ifndef _MENU_H
#define _MENU_H

class Menu {
private:
	TTF_Font * font;
	SDL_Color fontColor;
	SDL_Rect offset;
	SDL_Surface * bg;
	int selected_item;

	static const char * item[];
	static const int itemcount;
public:
	Menu();
	~Menu();
	void run();
	void draw();
};

#endif