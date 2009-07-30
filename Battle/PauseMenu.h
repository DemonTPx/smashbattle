#ifndef _PAUSEMENU_H
#define _PAUSEMENU_H

class PauseMenu {
public:
	PauseMenu(SDL_Surface * screen);
	~PauseMenu();

	int pause(Player * player);
	void add_option(char * name);

private:
	void process();
	void draw();

	SDL_Surface * screen;
	Player * player;

	bool paused;

	int draw_width;

	std::vector<char*> * options;
	unsigned int selected_option;
};

#endif
