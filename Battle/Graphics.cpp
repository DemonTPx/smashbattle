#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"

#include "Graphics.h"

Graphics::Graphics() {
}

Graphics::~Graphics() {
}

void Graphics::load_all() {
	SDL_Surface * surface;
	Uint32 colorkey;

	surface = SDL_LoadBMP("gfx/weapons.bmp");
	weapons = SDL_DisplayFormat(surface);
	colorkey = SDL_MapRGB(weapons->format, 0, 255, 255);
	SDL_SetColorKey(weapons, SDL_SRCCOLORKEY, colorkey); 
	SDL_FreeSurface(surface);

	surface = SDL_LoadBMP("gfx/bomb.bmp");
	bombs = SDL_DisplayFormat(surface);
	colorkey = SDL_MapRGB(bombs->format, 0, 255, 255);
	SDL_SetColorKey(bombs, SDL_SRCCOLORKEY, colorkey); 
	SDL_FreeSurface(surface);

	surface = SDL_LoadBMP("gfx/powerups.bmp");
	powerups = SDL_DisplayFormat(surface);
	colorkey = SDL_MapRGB(powerups->format, 0, 255, 255);
	SDL_SetColorKey(powerups, SDL_SRCCOLORKEY, colorkey); 
	SDL_FreeSurface(surface);

	surface = SDL_LoadBMP("gfx/player1hp.bmp");
	player1hp = SDL_DisplayFormat(surface);
	SDL_FreeSurface(surface);

	surface = SDL_LoadBMP("gfx/player2hp.bmp");
	player2hp = SDL_DisplayFormat(surface);
	SDL_FreeSurface(surface);

	font13 = TTF_OpenFont("fonts/slick.ttf", 13);
	font26 = TTF_OpenFont("fonts/slick.ttf", 26);
	font52 = TTF_OpenFont("fonts/slick.ttf", 52);

	white.r = 255;
	white.g = 255;
	white.b = 255;
}

void Graphics::clear_all() {
	SDL_FreeSurface(weapons);
	SDL_FreeSurface(bombs);
	SDL_FreeSurface(powerups);

	SDL_FreeSurface(player1hp);
	SDL_FreeSurface(player2hp);

	TTF_CloseFont(font13);
	TTF_CloseFont(font26);
	TTF_CloseFont(font52);
}
