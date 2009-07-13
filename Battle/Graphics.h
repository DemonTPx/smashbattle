#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"

class Graphics {
public:
	Graphics();
	~Graphics();
	
	void load_all();
	void clear_all();

	SDL_Surface * weapons;
	SDL_Surface * bombs;
	SDL_Surface * powerups;
	SDL_Surface * player1hp;
	SDL_Surface * player2hp;
	
	TTF_Font * font13;
	TTF_Font * font26;
	TTF_Font * font52;

	SDL_Color white;
};

#endif
