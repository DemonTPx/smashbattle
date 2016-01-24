#include "SimpleDrawable.h"
#include "Main.h"

SimpleDrawable::SimpleDrawable(Main &main) : main_(main) {}

void SimpleDrawable::draw()
{
	if (main_.no_sdl)
		return;

	draw_impl();
}
