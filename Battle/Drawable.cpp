#include "Drawable.h"
#include "Main.h"

#include "SDL2/SDL.h"

Drawable::Drawable(Main &main) : main_(main) {}

void Drawable::draw(SDL_Surface * screen, int frames_processed)
{
	if (main_.no_sdl)
		return;

	draw_impl(screen, frames_processed);
}
