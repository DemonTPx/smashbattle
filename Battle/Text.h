#ifndef _TEXT_H
#define _TEXT_H

class Text {
public:
	void load_all();
	void clear_all();

	SDL_Surface * render_glyph_small(const char g);
	SDL_Surface * render_glyph_medium(const char g);
	SDL_Surface * render_glyph_large(const char g);

	SDL_Surface * render_glyph_small_gray(const char g);
	SDL_Surface * render_glyph_medium_gray(const char g);
	SDL_Surface * render_glyph_large_gray(const char g);
	
	SDL_Surface * render_glyph_small_shadow(const char g);
	SDL_Surface * render_glyph_medium_shadow(const char g);
	SDL_Surface * render_glyph_large_shadow(const char g);
	
	SDL_Surface * render_text_small(const char * t);
	SDL_Surface * render_text_medium(const char * t);
	SDL_Surface * render_text_large(const char * t);
	
	SDL_Surface * render_text_small_gray(const char * t);
	SDL_Surface * render_text_medium_gray(const char * t);
	SDL_Surface * render_text_large_gray(const char * t);
	
	SDL_Surface * render_text_small_shadow(const char * t);
	SDL_Surface * render_text_medium_shadow(const char * t);
	SDL_Surface * render_text_large_shadow(const char * t);
private:
	SDL_Surface * font_small;
	SDL_Surface * font_medium;
	SDL_Surface * font_large;

	SDL_Surface * render_glyph(SDL_Surface * font, int w, int h, const char g, bool gray);
	SDL_Surface * render_text(SDL_Surface * font, int w, int h, int spacing, const char * t, bool gray);
	
	SDL_Surface * render_glyph_shadow(SDL_Surface * font, int w, int h, const char g, int offx, int offy);
	SDL_Surface * render_text_shadow(SDL_Surface * font, int w, int h, int spacing, const char * t, int offx, int offy);

	SDL_Rect * glyph_clip(int w, int h, const char g, bool gray);
};

#endif
