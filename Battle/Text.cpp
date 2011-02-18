#include <SDL/SDL.h>

#include "Text.h"

#define GLYPH_SMALL_W 5
#define GLYPH_SMALL_H 7
#define GLYPH_SMALL_SPACING 1
#define GLYPH_SMALL_SHADOW_OFFSET 1

#define GLYPH_MEDIUM_W 10
#define GLYPH_MEDIUM_H 14
#define GLYPH_MEDIUM_SPACING 2
#define GLYPH_MEDIUM_SHADOW_OFFSET 1

#define GLYPH_LARGE_W 20
#define GLYPH_LARGE_H 28
#define GLYPH_LARGE_SPACING 4
#define GLYPH_LARGE_SHADOW_OFFSET 2

#define GLYPS_PER_LINE 20
#define GLYPS_LINES 5

#define TEXT_ASCII_OFFSET 0x21
#define TEXT_ASCII_MAX 0x7e

#define TEXT_GLYPH_COUNT (TEXT_ASCII_MAX - TEXT_ASCII_OFFSET)

void Text::load_all() {
	SDL_Surface * surface;
	Uint32 colorkey;

	surface = SDL_LoadBMP("gfx/font_s.bmp");
	font_small = SDL_DisplayFormat(surface);
	colorkey = SDL_MapRGB(font_small->format, 0, 255, 255);
	SDL_SetColorKey(font_small, SDL_SRCCOLORKEY, colorkey); 
	SDL_FreeSurface(surface);
	
	surface = SDL_LoadBMP("gfx/font_m.bmp");
	font_medium = SDL_DisplayFormat(surface);
	colorkey = SDL_MapRGB(font_medium->format, 0, 255, 255);
	SDL_SetColorKey(font_medium, SDL_SRCCOLORKEY, colorkey); 
	SDL_FreeSurface(surface);
	
	surface = SDL_LoadBMP("gfx/font_l.bmp");
	font_large = SDL_DisplayFormat(surface);
	colorkey = SDL_MapRGB(font_large->format, 0, 255, 255);
	SDL_SetColorKey(font_large, SDL_SRCCOLORKEY, colorkey); 
	SDL_FreeSurface(surface);
}

void Text::clear_all() {
	SDL_FreeSurface(font_small);
	SDL_FreeSurface(font_medium);
	SDL_FreeSurface(font_large);
}

SDL_Rect * Text::glyph_clip(int w, int h, const char g, bool gray) {
	if(g < TEXT_ASCII_OFFSET || g > TEXT_ASCII_MAX)
		return NULL;

	int idx;
	SDL_Rect * rect;

	idx = (g - TEXT_ASCII_OFFSET);

	rect = new SDL_Rect();

	rect->x = (idx % GLYPS_PER_LINE) * w;
	rect->y = (idx / GLYPS_PER_LINE) * h;
	rect->w = w;
	rect->h = h;
	
	if(gray)
		rect->y += (GLYPS_LINES * h);

	return rect;
}

SDL_Surface * Text::render_glyph(SDL_Surface * font, int w, int h, const char g, bool gray) {
	if(g < TEXT_ASCII_OFFSET || g > TEXT_ASCII_MAX)
		return NULL;

	SDL_Surface * surface;
	SDL_Rect * rect;
	int idx;

	idx = (g - TEXT_ASCII_OFFSET);

	rect = glyph_clip(w, h, g, gray);

	surface = SDL_CreateRGBSurface(0, w, h, 32, 0xff000000, 0xff0000, 0xff00, 0xff);
	SDL_BlitSurface(font, rect, surface, 0);

	delete rect;

	return surface;
}

SDL_Surface * Text::render_glyph_shadow(SDL_Surface * font, int w, int h, const char g, int offx, int offy) {
	if(g < TEXT_ASCII_OFFSET || g > TEXT_ASCII_MAX)
		return NULL;

	SDL_Surface * surface;
	SDL_Rect * rect;
	SDL_Rect rect_d;
	int idx;

	idx = (g - TEXT_ASCII_OFFSET);

	surface = SDL_CreateRGBSurface(0, w + offx, h + offy, 32, 0xff000000, 0xff0000, 0xff00, 0xff);

	rect_d.x = offx;
	rect_d.y = offy;

	// Get grey glyph first
	rect = glyph_clip(w, h, g, true);
	SDL_BlitSurface(font, rect, surface, &rect_d);

	rect = glyph_clip(w, h, g, false);
	SDL_BlitSurface(font, rect, surface, 0);

	delete rect;

	return surface;
}

SDL_Surface * Text::render_text(SDL_Surface *font, int w, int h, int spacing, const char *t, bool gray) {
	SDL_Surface * surface;
	SDL_Rect * rect;
	SDL_Rect rect_d;
	int length;
	int surface_w;

	length = (int)strlen(t);

	surface_w = (w * length) + (spacing * (length - 1));

	surface = SDL_CreateRGBSurface(0, surface_w, h, 32, 0xff000000, 0xff0000, 0xff00, 0xff);
	
	rect_d.x = 0;
	rect_d.y = 0;
	rect_d.w = w;
	rect_d.h = h;

	for(int i = 0; i < length; i++) {
		if(t[i] >= TEXT_ASCII_OFFSET && t[i] <= TEXT_ASCII_MAX) {
			rect = glyph_clip(w, h, t[i], gray);

			SDL_BlitSurface(font, rect, surface, &rect_d);

			delete rect;
		}

		rect_d.x += w + spacing;
	}

	return surface;
}

SDL_Surface * Text::render_text_shadow(SDL_Surface * font, int w, int h, int spacing, const char * t, int offx, int offy) {
	SDL_Surface * surface;
	SDL_Rect * rect;
	SDL_Rect rect_d, rect_d_s;
	int length;
	int surface_w;

	length = (int)strlen(t);

	surface_w = (w * length) + (spacing * (length - 1)) + offx;

	surface = SDL_CreateRGBSurface(0, surface_w, h + offy, 32, 0xff000000, 0xff0000, 0xff00, 0xff);
	
	rect_d.x = 0;
	rect_d.y = 0;
	rect_d.w = w;
	rect_d.h = h;

	rect_d_s.w = w;
	rect_d_s.h = h;

	for(int i = 0; i < length; i++) {
		if(t[i] >= TEXT_ASCII_OFFSET && t[i] <= TEXT_ASCII_MAX) {
			rect_d_s.x = rect_d.x + offx;
			rect_d_s.y = rect_d.y + offy;
			rect = glyph_clip(w, h, t[i], true);
			SDL_BlitSurface(font, rect, surface, &rect_d_s);
			
			rect = glyph_clip(w, h, t[i], false);
			SDL_BlitSurface(font, rect, surface, &rect_d);

			delete rect;
		}

		rect_d.x += w + spacing;
	}

	return surface;
}

/* Glyph */
SDL_Surface * Text::render_glyph_small(const char g) {
	return render_glyph(font_small, GLYPH_SMALL_W, GLYPH_SMALL_H, g, false);
}

SDL_Surface * Text::render_glyph_medium(const char g) {
	return render_glyph(font_medium, GLYPH_MEDIUM_W, GLYPH_MEDIUM_H, g, false);
}

SDL_Surface * Text::render_glyph_large(const char g) {
	return render_glyph(font_large, GLYPH_LARGE_W, GLYPH_LARGE_H, g, false);
}

/* Glyph gray */
SDL_Surface * Text::render_glyph_small_gray(const char g) {
	return render_glyph(font_small, GLYPH_SMALL_W, GLYPH_SMALL_H, g, true);
}

SDL_Surface * Text::render_glyph_medium_gray(const char g) {
	return render_glyph(font_medium, GLYPH_MEDIUM_W, GLYPH_MEDIUM_H, g, true);
}

SDL_Surface * Text::render_glyph_large_gray(const char g) {
	return render_glyph(font_large, GLYPH_LARGE_W, GLYPH_LARGE_H, g, true);
}

/* Glyph gray */
SDL_Surface * Text::render_glyph_small_shadow(const char g) {
	return render_glyph_shadow(font_small, GLYPH_SMALL_W, GLYPH_SMALL_H, g, GLYPH_SMALL_SHADOW_OFFSET, GLYPH_SMALL_SHADOW_OFFSET);
}

SDL_Surface * Text::render_glyph_medium_shadow(const char g) {
	return render_glyph_shadow(font_medium, GLYPH_MEDIUM_W, GLYPH_MEDIUM_H, g, GLYPH_MEDIUM_SHADOW_OFFSET, GLYPH_MEDIUM_SHADOW_OFFSET);
}

SDL_Surface * Text::render_glyph_large_shadow(const char g) {
	return render_glyph_shadow(font_large, GLYPH_LARGE_W, GLYPH_LARGE_H, g, GLYPH_LARGE_SHADOW_OFFSET, GLYPH_LARGE_SHADOW_OFFSET);
}

/* Text */
SDL_Surface * Text::render_text_small(const char * t) {
	return render_text(font_small, GLYPH_SMALL_W, GLYPH_SMALL_H, GLYPH_SMALL_SPACING, t, false);
}

SDL_Surface * Text::render_text_medium(const char * t) {
	return render_text(font_medium, GLYPH_MEDIUM_W, GLYPH_MEDIUM_H, GLYPH_MEDIUM_SPACING, t, false);
}

SDL_Surface * Text::render_text_large(const char * t) {
	return render_text(font_large, GLYPH_LARGE_W, GLYPH_LARGE_H, GLYPH_LARGE_SPACING, t, false);
}

/* Text Gray */
SDL_Surface * Text::render_text_small_gray(const char * t) {
	return render_text(font_small, GLYPH_SMALL_W, GLYPH_SMALL_H, GLYPH_SMALL_SPACING, t, true);
}

SDL_Surface * Text::render_text_medium_gray(const char * t) {
	return render_text(font_medium, GLYPH_MEDIUM_W, GLYPH_MEDIUM_H, GLYPH_MEDIUM_SPACING, t, true);
}

SDL_Surface * Text::render_text_large_gray(const char * t) {
	return render_text(font_large, GLYPH_LARGE_W, GLYPH_LARGE_H, GLYPH_LARGE_SPACING, t, true);
}

/* Text Shadow */
SDL_Surface * Text::render_text_small_shadow(const char * t) {
	return render_text_shadow(font_small, GLYPH_SMALL_W, GLYPH_SMALL_H, GLYPH_SMALL_SPACING, t, GLYPH_SMALL_SHADOW_OFFSET, GLYPH_SMALL_SHADOW_OFFSET);
}

SDL_Surface * Text::render_text_medium_shadow(const char * t) {
	return render_text_shadow(font_medium, GLYPH_MEDIUM_W, GLYPH_MEDIUM_H, GLYPH_MEDIUM_SPACING, t, GLYPH_MEDIUM_SHADOW_OFFSET, GLYPH_MEDIUM_SHADOW_OFFSET);
}

SDL_Surface * Text::render_text_large_shadow(const char * t) {
	return render_text_shadow(font_large, GLYPH_LARGE_W, GLYPH_LARGE_H, GLYPH_LARGE_SPACING, t, GLYPH_LARGE_SHADOW_OFFSET, GLYPH_LARGE_SHADOW_OFFSET);
}
