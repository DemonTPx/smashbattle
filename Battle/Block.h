#ifndef _BLOCK_H
#define _BLOCK_H

class Block {
private:
	SDL_Surface * surf;
public:
	int x, y;
	int w, h;
	int xSpeed, ySpeed;

	Block();
	~Block();
	void handle_input(SDL_Event * event);
	void move();
	void show(SDL_Surface * screen);
	SDL_Rect * get_rect();
};

#endif

