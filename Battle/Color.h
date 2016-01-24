#pragma once

#define SDL_MapRGBHex(format, color) SDL_MapRGB(format, ((color >> 16) & 0xff), ((color >> 8) & 0xff), color & 0xff)
#define SDL_FillRectColor(surface, rect, color) SDL_FillRect(surface, rect, SDL_MapRGBHex(surface->format, color))
