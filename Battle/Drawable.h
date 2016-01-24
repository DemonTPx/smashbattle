#pragma once

class SDL_Surface;
class Main;

class Drawable
{
public:

	void draw(SDL_Surface * screen, int frames_processed = 0);
	//void draw();

protected:

	Drawable(Main &main);

	virtual void draw_impl(SDL_Surface * screen, int frames_processed) = 0;
	//virtual void draw_impl() = 0;

	Main &main_;
};