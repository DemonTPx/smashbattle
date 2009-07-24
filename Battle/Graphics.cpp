#include "SDL/SDL.h"

#include "Player.h"
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

	surface = SDL_LoadBMP("gfx/common.bmp");
	common = SDL_DisplayFormat(surface);
	colorkey = SDL_MapRGB(common->format, 0, 255, 255);
	SDL_SetColorKey(common, SDL_SRCCOLORKEY, colorkey); 
	SDL_FreeSurface(surface);

	surface = SDL_LoadBMP("gfx/pmarkers.bmp");
	pmarker = SDL_DisplayFormat(surface);
	SDL_FreeSurface(surface);
	colorkey = SDL_MapRGB(pmarker->format, 0, 255, 255);
	SDL_SetColorKey(pmarker, SDL_SRCCOLORKEY, colorkey);
	
	surface = SDL_LoadBMP("gfx/bg_grey.bmp");
	bg_grey = SDL_DisplayFormat(surface);
	SDL_FreeSurface(surface);

	surface = SDL_LoadBMP("gfx/title_screen.bmp");
	bg_menu = SDL_DisplayFormat(surface);
	SDL_FreeSurface(surface);

	surface = SDL_LoadBMP("gfx/cups.bmp");
	cups = SDL_DisplayFormat(surface);
	SDL_FreeSurface(surface);
	colorkey = SDL_MapRGB(cups->format, 0, 255, 255);
	SDL_SetColorKey(cups, SDL_SRCCOLORKEY, colorkey);

	surface = SDL_LoadBMP("gfx/tiles.bmp");
	tiles = SDL_DisplayFormat(surface);
	SDL_FreeSurface(surface);
	colorkey = SDL_MapRGB(tiles->format, 0, 255, 255);
	SDL_SetColorKey(tiles, SDL_SRCCOLORKEY, colorkey);

	load_players();
	set_player_clips();
}

void Graphics::load_players() {
	SDL_Surface * loaded, * surface;
	Uint32 colorkey;

	player = new std::vector<SDL_Surface *>(0);

	for(int i = 0; i < Player::CHARACTER_COUNT; i++) {
		loaded = SDL_LoadBMP(Player::CHARACTERS[i].filename);
		surface = SDL_DisplayFormat(loaded);
		SDL_FreeSurface(loaded);
		colorkey = SDL_MapRGB(surface->format, 0, 255, 255);
		SDL_SetColorKey(surface, SDL_SRCCOLORKEY, colorkey); 
		player->push_back(surface);
	}
}

void Graphics::clear_all() {
	SDL_FreeSurface(weapons);
	SDL_FreeSurface(bombs);
	SDL_FreeSurface(powerups);

	SDL_FreeSurface(player1hp);
	SDL_FreeSurface(player2hp);

	SDL_FreeSurface(common);

	SDL_FreeSurface(pmarker);

	SDL_FreeSurface(bg_grey);
	SDL_FreeSurface(bg_menu);
	
	SDL_FreeSurface(cups);

	SDL_FreeSurface(tiles);

	clear_players();
	clear_player_clips();
}

void Graphics::clear_players() {
	for(unsigned int i = 0; i < player->size(); i++) {
		SDL_FreeSurface(player->at(i));
	}
	player->clear();
	delete player;
}

void Graphics::set_player_clips() {
	int row_width = PLAYER_W * PLAYER_SURF_COLS;
	for (int i = 0; i < PLAYER_SURF_COUNT; i++) {
		player_clip[i] = new SDL_Rect();
		player_clip[i]->w = PLAYER_W;
		player_clip[i]->h = PLAYER_H;
		player_clip[i]->x = (i * PLAYER_W) % row_width;
		player_clip[i]->y = (int)(i / PLAYER_SURF_COLS) * PLAYER_H;
	}

	player_clip[SPR_AVATAR] = new SDL_Rect();
	player_clip[SPR_AVATAR]->x = row_width;
	player_clip[SPR_AVATAR]->y = 0;
	player_clip[SPR_AVATAR]->w = PLAYER_W * 2;
	player_clip[SPR_AVATAR]->h = PLAYER_H;

	player_clip[SPR_AVATAR_SELECTED] = new SDL_Rect();
	player_clip[SPR_AVATAR_SELECTED]->x = row_width;
	player_clip[SPR_AVATAR_SELECTED]->y = player_clip[SPR_AVATAR]->h;
	player_clip[SPR_AVATAR_SELECTED]->w = PLAYER_W * 2;
	player_clip[SPR_AVATAR_SELECTED]->h = PLAYER_H;

	for(int i = 0; i < 4; i++) {
		pmarker_clip[i] = new SDL_Rect();
		pmarker_clip[i]->x = 16 * i;
		pmarker_clip[i]->y = 0;
		pmarker_clip[i]->w = 16;
		pmarker_clip[i]->h = 20;
	}
}

void Graphics::clear_player_clips() {
	for(int i = 0; i < SPR_COUNT; i++) {
		delete player_clip[i];
	}
	for(int i = 0; i < 4; i++) {
		delete pmarker_clip[i];
	}
}

Uint32 Graphics::combine_colors(Uint32 color1, Uint32 color2) {
	int r1, g1, b1;
	int r2, g2, b2;
	r1 = (color1 & 0xff0000) >> 16;
	g1 = (color1 & 0xff00) >> 8;
	b1 = color1 & 0xff;
	r2 = (color2 & 0xff0000) >> 16;
	g2 = (color2 & 0xff00) >> 8;
	b2 = color2 & 0xff;
	return (((r1 + r2) / 2) << 16) + (((g1 + g2) / 2) << 8) + ((b1 + b2) / 2);
}
