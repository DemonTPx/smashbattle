#pragma once

#include "SimpleDrawable.h"

class PauseMenu : public SimpleDrawable {
public:
	PauseMenu(SDL_Surface * screen, Main &main);
	~PauseMenu();

	int pause(Player * player);
	void add_option(char * name);

protected:

	virtual void draw_impl();

private:
	void process();

	SDL_Surface * screen;
	Player * player;

	bool paused;

	int draw_width;

	std::vector<char*> * options;
	unsigned int selected_option;

	Main &main_;
};
