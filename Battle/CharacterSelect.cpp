#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"

#include "Main.h"
#include "Battle.h"
#include "Player.h"

#include "CharacterSelect.h"

CharacterSelect::CharacterSelect() {

}

void CharacterSelect::run() {
	SDL_Event event;
	int select1, select2;
	ControlScheme controls1, controls2;

	load_fonts();

	ready1 = false;
	ready2 = false;

	controls1 = Main::instance->controls1;
	controls2 = Main::instance->controls2;

	select1 = 0;
	select2 = Battle::CHARACTER_COUNT - 1;
	while (Main::running && (!ready1 || !ready2)) {
		while(SDL_PollEvent(&event)) {
			Main::instance->handle_event(&event);
			if(event.type == SDL_KEYDOWN) {
				if(controls1.use_keyboard) {
					if(event.key.keysym.sym == controls1.kb_left && !ready1) {
						select1 -= 1;
						if(select1 < 0) select1 = Battle::CHARACTER_COUNT - 1;
					}
					else if(event.key.keysym.sym == controls1.kb_right && !ready1) {
						select1 += 1;
						if(select1 >= Battle::CHARACTER_COUNT) select1 = 0;
					}
					else if(event.key.keysym.sym == controls1.kb_shoot || 
						event.key.keysym.sym == controls1.kb_run ||
						event.key.keysym.sym == controls1.kb_jump) {
							ready1 = !ready1;
					}
				}
				if(controls2.use_keyboard) {
					if(event.key.keysym.sym == controls2.kb_left && !ready2) {
						select2 -= 1;
						if(select2 < 0) select2 = Battle::CHARACTER_COUNT - 1;
					}
					else if(event.key.keysym.sym == controls2.kb_right && !ready2) {
						select2 += 1;
						if(select2 >= Battle::CHARACTER_COUNT) select2 = 0;
					}
					else if(event.key.keysym.sym == controls2.kb_shoot || 
						event.key.keysym.sym == controls2.kb_run ||
						event.key.keysym.sym == controls2.kb_jump) {
							ready2 = !ready2;
					}
				}
			}
			if(event.type == SDL_JOYBUTTONDOWN) {
				if(controls1.use_joystick && event.jbutton.which == controls1.joystick_idx) {
					if(event.jbutton.button == controls1.js_left && !ready1) {
						select1 -= 1;
						if(select1 < 0) select1 = Battle::CHARACTER_COUNT - 1;
					}
					if(event.jbutton.button == controls1.js_right && !ready1) {
						select1 += 1;
						if(select1 >= Battle::CHARACTER_COUNT) select1 = 0;
					}
					if(event.jbutton.button == controls1.js_jump ||
						event.jbutton.button == controls1.js_run ||
						event.jbutton.button == controls1.js_shoot) {
							ready1 = !ready1;
					}
				}
				if(controls2.use_joystick && event.jbutton.which == controls2.joystick_idx) {
					if(event.jbutton.button == controls2.js_left && !ready2) {
						select2 -= 1;
						if(select2 < 0) select2 = Battle::CHARACTER_COUNT - 1;
					}
					if(event.jbutton.button == controls2.js_right && !ready2) {
						select2 += 1;
						if(select2 >= Battle::CHARACTER_COUNT) select2 = 0;
					}
					if(event.jbutton.button == controls2.js_jump ||
						event.jbutton.button == controls2.js_run ||
						event.jbutton.button == controls2.js_shoot) {
							ready2 = !ready2;
					}
				}
			}
			if(event.type == SDL_JOYAXISMOTION) {
				if(controls1.use_joystick && event.jbutton.which == controls1.joystick_idx) {
					if(event.jaxis.value < -6400 && !ready1) {
						select1 -= 1;
						if(select1 < 0) select1 = Battle::CHARACTER_COUNT - 1;
					}
					else if(event.jaxis.value > 6400 && !ready1) {
						select1 += 1;
						if(select1 >= Battle::CHARACTER_COUNT) select1 = 0;
					}
				}
				if(controls2.use_joystick && event.jbutton.which == controls2.joystick_idx) {
					if(event.jaxis.value < -6400 && !ready2) {
						select2 -= 1;
						if(select2 < 0) select2 = Battle::CHARACTER_COUNT - 1;
					}
					else if(event.jaxis.value > 6400 && !ready2) {
						select2 += 1;
						if(select2 >= Battle::CHARACTER_COUNT) select2 = 0;
					}
				}
			}
		}

		name1 = Battle::characters[select1].name;
		name2 = Battle::characters[select2].name;

		file1 = Battle::characters[select1].filename;
		file2 = Battle::characters[select2].filename;

		draw();


		Main::instance->flip();
	}

	free_fonts();
}

void CharacterSelect::draw() {
	SDL_Surface * screen;
	SDL_Surface * surface;
	SDL_Rect rect;
	SDL_Surface * loaded;
	Uint32 colorkey;
	SDL_Rect rect_s;

	screen = Main::instance->screen;

	SDL_FillRect(screen, NULL, 0);

	loaded = SDL_LoadBMP(file1);

	surface = SDL_DisplayFormat(loaded);
	SDL_FreeSurface(loaded);
	colorkey = SDL_MapRGB(surface->format, 255, 255, 255);
	SDL_SetColorKey(surface, SDL_SRCCOLORKEY, colorkey);
	rect.x = 40;
	rect.y = 40;
	rect_s.x = 0;
	rect_s.y = 0;
	rect_s.w = PLAYER_W;
	rect_s.h = PLAYER_H;
	SDL_BlitSurface(surface, &rect_s, screen, &rect);
	SDL_FreeSurface(surface);

	loaded = SDL_LoadBMP(file2);
	surface = SDL_DisplayFormat(loaded);
	SDL_FreeSurface(loaded);
	colorkey = SDL_MapRGB(surface->format, 255, 255, 255);
	SDL_SetColorKey(surface, SDL_SRCCOLORKEY, colorkey);
	rect.x = screen->clip_rect.w - PLAYER_W - 40;
	rect.y = 40;
	rect_s.x = 0;
	rect_s.y = PLAYER_H;
	rect_s.w = PLAYER_W;
	rect_s.h = PLAYER_H;
	SDL_BlitSurface(surface, &rect_s, screen, &rect);
	SDL_FreeSurface(surface);

	surface = TTF_RenderText_Solid(font13, name1, fontColor);
	rect.x = 50 + PLAYER_W;
	rect.y = 40 + ((PLAYER_H - surface->clip_rect.h)/ 2);
	SDL_BlitSurface(surface, NULL, screen, &rect);
	SDL_FreeSurface(surface);

	surface = TTF_RenderText_Solid(font13, name2, fontColor);
	rect.x = screen->clip_rect.w - PLAYER_W - surface->clip_rect.w - 50;
	rect.y = 40 + ((PLAYER_H - surface->clip_rect.h)/ 2);
	SDL_BlitSurface(surface, NULL, screen, &rect);
	SDL_FreeSurface(surface);

	if(ready1) {
		surface = TTF_RenderText_Solid(font13, "READY", fontColor);
		rect.x = 50 + PLAYER_W;
		rect.y = 50 + ((PLAYER_H - surface->clip_rect.h)/ 2);
		SDL_BlitSurface(surface, NULL, screen, &rect);
		SDL_FreeSurface(surface);
	}

	if(ready2) {
		surface = TTF_RenderText_Solid(font13, "READY", fontColor);
		rect.x = screen->clip_rect.w - PLAYER_W - surface->clip_rect.w - 50;
		rect.y = 50 + ((PLAYER_H - surface->clip_rect.h)/ 2);
		SDL_BlitSurface(surface, NULL, screen, &rect);
		SDL_FreeSurface(surface);
	}
}

void CharacterSelect::load_fonts() {
	font26 = TTF_OpenFont("fonts/slick.ttf", 26);
	font13 = TTF_OpenFont("fonts/slick.ttf", 13);
	fontColor.r = 255;
	fontColor.g = 255;
	fontColor.b = 255;
}

void CharacterSelect::free_fonts() {
	TTF_CloseFont(font26);
	TTF_CloseFont(font13);
}
