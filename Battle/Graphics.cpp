#include "SDL/SDL.h"

#include "Player.h"
#include "Graphics.h"
#include "Main.h"
#include "Color.h"

Graphics::Graphics(Main &main) : main_(main) {
}

Graphics::~Graphics() {
}

void Graphics::load_all() {
	weapons = load_bmp("gfx/weapons.bmp");
	kill_moves = load_bmp("gfx/kill_moves.bmp");
	bombs = load_bmp("gfx/bomb.bmp");
	owl = load_bmp("gfx/owl.bmp");
	powerups = load_bmp("gfx/powerups.bmp");

	shield = load_bmp("gfx/shield.bmp");

	player1hp = load_bmp("gfx/player1hp.bmp");
	player2hp = load_bmp("gfx/player2hp.bmp");

	common = load_bmp("gfx/common.bmp");
	pmarker = load_bmp("gfx/pmarkers.bmp");
	
	bg_grey = load_bmp("gfx/bg_grey.bmp");

#ifdef PBWEB
#ifdef PBWEB_JUNGLE
	bg_menu = Level::get_preview("stage/titlescreen_jungle.lvl");
#elif defined(PBWEB_DPC)
	bg_menu = Level::get_preview("stage/titlescreen_dpc.lvl");
#else
	bg_menu = Level::get_preview("stage/titlescreen.lvl");
#endif
#elif defined(TWEAKERS)
	bg_menu = Level::get_preview("stage/titlescreen_tweakers.lvl");
#else
	bg_menu = Level::get_preview("stage/titlescreen.lvl");
#endif

	cups = load_bmp("gfx/cups.bmp");

	tiles = load_bmp("gfx/tiles.bmp");
	
	npc_chick = load_bmp("gfx/chick.bmp");
	npc_cannon = load_bmp("gfx/cannon.bmp");
	npc_gatling = load_bmp("gfx/gatling.bmp");

	statsblock[0] = SDL_CreateRGBSurface(0, 16, 18, 32, 0, 0, 0, 0);
	SDL_FillRectColor(statsblock[0], NULL, 0x880000);

	statsblock[1] = SDL_CreateRGBSurface(0, 16, 18, 32, 0, 0, 0, 0);
	SDL_FillRectColor(statsblock[1], NULL, 0x888800);

	statsblock[2] = SDL_CreateRGBSurface(0, 16, 18, 32, 0, 0, 0, 0);
	SDL_FillRectColor(statsblock[2], NULL, 0x008800);

	text_ready = main_.text->render_text_medium("READY");
	text_random = main_.text->render_text_medium("RANDOM");
	
	text_pressstart = main_.text->render_text_medium("PRESS START");

	load_players();
	set_player_clips();
}

SDL_Surface * Graphics::load_bmp(const char * filename) {
	SDL_Surface * loaded, * surface;
	Uint32 colorkey;

	loaded = SDL_LoadBMP(filename);
	if(loaded == NULL) {
		printf("Could not load '%s'", filename);
		return NULL;
	}
	surface = SDL_DisplayFormat(loaded);
	SDL_FreeSurface(loaded);
	colorkey = SDL_MapRGB(surface->format, 0, 255, 255);
	SDL_SetColorKey(surface, SDL_SRCCOLORKEY, colorkey);

	return surface;
}

void Graphics::load_players() {
	SDL_Surface * loaded, * surface;
	Uint32 colorkey;

	player = new std::vector<SDL_Surface *>(0);
	playername = new std::vector<SDL_Surface *>(0);

	for(int i = 0; i < Player::CHARACTER_COUNT; i++) {
		loaded = SDL_LoadBMP(Player::CHARACTERS[i].filename);
		surface = SDL_DisplayFormat(loaded);
		SDL_FreeSurface(loaded);
		colorkey = SDL_MapRGB(surface->format, 0, 255, 255);
		SDL_SetColorKey(surface, SDL_SRCCOLORKEY, colorkey); 
		player->push_back(surface);

		playername->push_back(main_.text->render_text_medium(Player::CHARACTERS[i].name));
	}

}

void Graphics::replace_color(SDL_Surface * surface, Uint32 color_old, Uint32 color_new) {
	Uint32 pixel;
	Uint32 * p;
	Uint8 * pixels;
	int pixelcount;
	int mask;

	color_old = SDL_MapRGBHex(surface->format, color_old);
	color_new = SDL_MapRGBHex(surface->format, color_new);

	pixels = (Uint8 *)surface->pixels;
	pixelcount = surface->w * surface->h;

	mask = surface->format->Rmask | surface->format->Gmask | surface->format->Bmask;

	for (int x = 0; x < pixelcount * surface->format->BytesPerPixel; x += surface->format->BytesPerPixel) {
		pixel = *((Uint32 *)(pixels + x)) & mask;
		p = ((Uint32 *)(pixels + x));
		if (pixel == color_old) {
			*p = color_new;
		}
	}
}

/*
void Graphics::create_player_masks() {
	SDL_Surface * surface;
	Uint32 colorkey;
	Uint32 maskcolor;
	Uint32 pixel;
	Uint32 * p;
	Uint8 * pixels;
	int pixelcount;
	int mask;

	player_poisoned = new std::vector<SDL_Surface *>();

	for(int i = 0; i < Player::CHARACTER_COUNT; i++) {
		surface = SDL_DisplayFormat(player->at(i));

		pixels = (Uint8*)surface->pixels;
		colorkey = SDL_MapRGB(surface->format, 0, 255, 255);
		maskcolor = SDL_MapRGB(surface->format, 0, 128, 0);

		pixelcount = surface->w * surface->h;

		mask = surface->format->Rmask | surface->format->Gmask | surface->format->Bmask;

		for(int x = 0; x < pixelcount * surface->format->BytesPerPixel; x += surface->format->BytesPerPixel) {
			pixel = *((Uint32 *)(pixels + x)) & mask;
			p = ((Uint32 *)(pixels + x));
			if(pixel != colorkey) {
				*p = maskcolor;
			}
		}

		player_poisoned->push_back(surface);
	}
}
*/

void Graphics::clear_all() {
	SDL_FreeSurface(weapons);
	SDL_FreeSurface(kill_moves);
	SDL_FreeSurface(bombs);
	SDL_FreeSurface(owl);
	SDL_FreeSurface(powerups);

	SDL_FreeSurface(shield);

	SDL_FreeSurface(player1hp);
	SDL_FreeSurface(player2hp);

	SDL_FreeSurface(common);

	SDL_FreeSurface(pmarker);

	SDL_FreeSurface(bg_grey);
	SDL_FreeSurface(bg_menu);
	
	SDL_FreeSurface(cups);

	SDL_FreeSurface(tiles);

	SDL_FreeSurface(npc_chick);
	SDL_FreeSurface(npc_cannon);
	SDL_FreeSurface(npc_gatling);

	SDL_FreeSurface(statsblock[0]);
	SDL_FreeSurface(statsblock[1]);
	SDL_FreeSurface(statsblock[2]);

	SDL_FreeSurface(text_ready);
	SDL_FreeSurface(text_random);

	SDL_FreeSurface(text_pressstart);

	clear_players();
	clear_player_clips();
}

void Graphics::clear_players() {
	for(unsigned int i = 0; i < player->size(); i++) {
		SDL_FreeSurface(player->at(i));
	}
	player->clear();
	delete player;
	
	for(unsigned int i = 0; i < playername->size(); i++) {
		SDL_FreeSurface(playername->at(i));
	}
	playername->clear();
	delete playername;
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

	player_clip[SPR_R_HEAD] = new SDL_Rect();
	player_clip[SPR_R_HEAD]->x = 0;
	player_clip[SPR_R_HEAD]->y = 0;
	player_clip[SPR_R_HEAD]->w = PLAYER_W;
	player_clip[SPR_R_HEAD]->h = PLAYER_W;

	player_clip[SPR_L_HEAD] = new SDL_Rect();
	player_clip[SPR_L_HEAD]->x = 0;
	player_clip[SPR_L_HEAD]->y = player_clip[SPR_L_HEAD]->h;
	player_clip[SPR_L_HEAD]->w = PLAYER_W;
	player_clip[SPR_L_HEAD]->h = PLAYER_W;

	for(int i = 0; i < 4; i++) {
		pmarker_clip_below[i] = new SDL_Rect();
		pmarker_clip_below[i]->x = 14 * i;
		pmarker_clip_below[i]->y = 0;
		pmarker_clip_below[i]->w = 14;
		pmarker_clip_below[i]->h = 20;

		pmarker_clip_above[i] = new SDL_Rect();
		pmarker_clip_above[i]->x = 14 * i;
		pmarker_clip_above[i]->y = 20;
		pmarker_clip_above[i]->w = 14;
		pmarker_clip_above[i]->h = 20;
	}
}

void Graphics::clear_player_clips() {
	for(int i = 0; i < SPR_COUNT; i++) {
		delete player_clip[i];
	}
	for(int i = 0; i < 4; i++) {
		delete pmarker_clip_above[i];
		delete pmarker_clip_below[i];
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

	return (Uint32) (((r1 + r2) / 2) << 16) + (((g1 + g2) / 2) << 8) + ((b1 + b2) / 2);
}

SDL_Surface * Graphics::load_icon(const char * filename, Uint8 ** mask, int color) {
	SDL_Surface * icon;
	Uint8 *pixels;
	Uint32 this_pixel;
	Uint32 mapped_color;
	int num_pixels, pos;
    Uint8 r, g, b;

	*mask = NULL;

	icon = SDL_LoadBMP(filename);
	mapped_color = SDL_MapRGBHex(icon->format, color);
	pixels = (Uint8*)icon->pixels;
	num_pixels = icon->w * icon->h;
	*mask = new Uint8[num_pixels / 8];
	memset(*mask, 0, (size_t) num_pixels / 8);

	for (int i = 0; i < num_pixels; i ++) {
        pos = i * icon->format->BytesPerPixel;

        r = *(pixels + pos);
        g = *(pixels + pos + 1);
        b = *(pixels + pos + 2);

        this_pixel = (Uint32)r | (Uint32)g << 8 | (Uint32)b << 16;

		if (this_pixel != mapped_color) {
            (*mask)[i / 8] |= 0x80 >> i % 8;
        }
	}

	return icon;
}
