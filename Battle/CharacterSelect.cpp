#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"

#include "Main.h"
#include "Battle.h"
#include "Player.h"

#include "CharacterSelect.h"

#define CHARACTERS_PER_LINE 2
#define CHARACTER_WIDTH 44
#define CHARACTER_SPACING 4

CharacterSelect::CharacterSelect() {

}

void CharacterSelect::run() {
	SDL_Event event;
	ControlScheme controls1, controls2;

	load_fonts();
	load_sprites();

	ready1 = false;
	ready2 = false;

	controls1 = Main::instance->controls1;
	controls2 = Main::instance->controls2;

	select1 = 0;
	if(Battle::CHARACTER_COUNT >= CHARACTERS_PER_LINE)
		select2 = CHARACTERS_PER_LINE - 1;
	else
		select2 = Battle::CHARACTER_COUNT;

	while (Main::running && (!ready1 || !ready2)) {
		while(SDL_PollEvent(&event)) {
			Main::instance->handle_event(&event);
			if(event.type == SDL_KEYDOWN) {
				if(controls1.use_keyboard) {
					if(event.key.keysym.sym == controls1.kb_left && !ready1) {
						if(select1 % CHARACTERS_PER_LINE == 0)
							select1 += CHARACTERS_PER_LINE;
						select1 -= 1;
					}
					else if(event.key.keysym.sym == controls1.kb_right && !ready1) {
						if((select1 + 1) % CHARACTERS_PER_LINE == 0)
							select1 -= CHARACTERS_PER_LINE;
						select1 += 1;
					}
					if(event.key.keysym.sym == controls1.kb_up && !ready1) {
						select1 -= CHARACTERS_PER_LINE;
					}
					else if(event.key.keysym.sym == controls1.kb_down && !ready1) {
						select1 += CHARACTERS_PER_LINE;
					}
					else if(event.key.keysym.sym == controls1.kb_shoot || 
						event.key.keysym.sym == controls1.kb_run ||
						(controls1.kb_up != controls1.kb_jump &&
						event.key.keysym.sym == controls1.kb_jump)) {
							ready1 = !ready1;
					}
				}
				if(controls2.use_keyboard) {
					if(event.key.keysym.sym == controls2.kb_left && !ready2) {
						if(select2 % CHARACTERS_PER_LINE == 0)
							select2 += CHARACTERS_PER_LINE;
						select2 -= 1;
					}
					else if(event.key.keysym.sym == controls2.kb_right && !ready2) {
						if((select2 + 1) % CHARACTERS_PER_LINE == 0)
							select2 -= CHARACTERS_PER_LINE;
						select2 += 1;
					}
					if(event.key.keysym.sym == controls2.kb_up && !ready1) {
						select2 -= CHARACTERS_PER_LINE;
					}
					else if(event.key.keysym.sym == controls2.kb_down && !ready1) {
						select2 += CHARACTERS_PER_LINE;
					}
					else if(event.key.keysym.sym == controls2.kb_shoot || 
						event.key.keysym.sym == controls2.kb_run ||
						(controls2.kb_up != controls2.kb_jump &&
						event.key.keysym.sym == controls2.kb_jump)) {
							ready2 = !ready2;
					}
				}
			}
			if(event.type == SDL_JOYBUTTONDOWN) {
				if(controls1.use_joystick && event.jbutton.which == controls1.joystick_idx) {
					if(event.jbutton.button == controls1.js_left && !ready1) {
						if(select1 % CHARACTERS_PER_LINE == 0)
							select1 += CHARACTERS_PER_LINE;
						select1 -= 1;
					}
					if(event.jbutton.button == controls1.js_right && !ready1) {
						if((select1 + 1) % CHARACTERS_PER_LINE == 0)
							select1 -= CHARACTERS_PER_LINE;
						select1 += 1;
					}
					if(event.jbutton.button == controls1.js_jump ||
						event.jbutton.button == controls1.js_run ||
						event.jbutton.button == controls1.js_shoot) {
							ready1 = !ready1;
					}
				}
				if(controls2.use_joystick && event.jbutton.which == controls2.joystick_idx) {
					if(event.jbutton.button == controls2.js_left && !ready2) {
						if(select2 % CHARACTERS_PER_LINE == 0)
							select2 += CHARACTERS_PER_LINE;
						select2 -= 1;
					}
					if(event.jbutton.button == controls2.js_right && !ready2) {
						if((select2 + 1) % CHARACTERS_PER_LINE == 0)
							select2 -= CHARACTERS_PER_LINE;
						select2 += 1;
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
					if(event.jaxis.axis == 0) {
						if(event.jaxis.value < -6400 && !ready1) {
							if(select1 % CHARACTERS_PER_LINE == 0)
								select1 += CHARACTERS_PER_LINE;
							select1 -= 1;
						}
						else if(event.jaxis.value > 6400 && !ready1) {
							if((select1 + 1) % CHARACTERS_PER_LINE == 0)
								select1 -= CHARACTERS_PER_LINE;
							select1 += 1;
						}
					} else {
						if(event.jaxis.value < -6400 && !ready1) {
							select1 -= CHARACTERS_PER_LINE;
						}
						else if(event.jaxis.value > 6400 && !ready1) {
							select1 += CHARACTERS_PER_LINE;
						}
					}
				}
				if(controls2.use_joystick && event.jbutton.which == controls2.joystick_idx) {
					if(event.jaxis.axis == 0) {
						if(event.jaxis.value < -6400 && !ready2) {
							if(select2 % CHARACTERS_PER_LINE == 0)
								select2 += CHARACTERS_PER_LINE;
							select2 -= 1;
						}
						else if(event.jaxis.value > 6400 && !ready2) {
							if((select2 + 1) % CHARACTERS_PER_LINE == 0)
								select2 -= CHARACTERS_PER_LINE;
							select2 += 1;
						}
					} else {
						if(event.jaxis.value < -6400 && !ready2) {
							select2 -= CHARACTERS_PER_LINE;
						}
						else if(event.jaxis.value > 6400 && !ready2) {
							select2 += CHARACTERS_PER_LINE;
						}
					}
				}
			}

			while(select1 < Battle::CHARACTER_COUNT) {
				select1 += Battle::CHARACTER_COUNT;
			}
			while(select1 >= Battle::CHARACTER_COUNT) {
				select1 -= Battle::CHARACTER_COUNT;
			}

			while(select2 < Battle::CHARACTER_COUNT) {
				select2 += Battle::CHARACTER_COUNT;
			}
			while(select2 >= Battle::CHARACTER_COUNT) {
				select2 -= Battle::CHARACTER_COUNT;
			}
		}

		name1 = Battle::characters[select1].name;
		name2 = Battle::characters[select2].name;

		file1 = Battle::characters[select1].filename;
		file2 = Battle::characters[select2].filename;

		draw();


		Main::instance->flip();
	}

	free_sprites();
	free_fonts();
}

void CharacterSelect::draw() {
	SDL_Surface * screen;
	SDL_Surface * surface;
	SDL_Rect rect, rect_b;

	screen = Main::instance->screen;

	SDL_FillRect(screen, NULL, 0);

	rect_b.x = (screen->clip_rect.w - ((clip_avatar->w + (CHARACTER_SPACING * 2)) * CHARACTERS_PER_LINE)) / 2;
	rect_b.y = 20;
	rect_b.w = clip_avatar->w + (CHARACTER_SPACING * 2);
	rect_b.h = clip_avatar->h + (CHARACTER_SPACING * 2);

	for(int idx = 0; idx < Battle::CHARACTER_COUNT; idx++) {
		if(idx > 0 && idx % CHARACTERS_PER_LINE == 0) {
			rect_b.x = (screen->clip_rect.w - ((clip_avatar->w + (CHARACTER_SPACING * 2)) * CHARACTERS_PER_LINE)) / 2;
			rect_b.y += rect_b.h;
		}

		rect.x = rect_b.x + CHARACTER_SPACING;
		rect.y = rect_b.y + CHARACTER_SPACING;

		if(select1 == idx) {
			SDL_FillRect(screen, &rect_b, 0xff0000);
		}
		if(select2 == idx) {
			SDL_FillRect(screen, &rect_b, 0x0000ff);
		}
		if(select1 == idx && select2 == idx) {
			SDL_FillRect(screen, &rect_b, 0xff00ff);
		}

		SDL_BlitSurface(character_sprites->at(idx), clip_avatar, screen, &rect);

		rect_b.x += clip_avatar->w + (CHARACTER_SPACING * 2);
	}

	// Player 1
	rect.x = 40;
	rect.y = 40;
	SDL_BlitSurface(character_sprites->at(select1), clip_left, screen, &rect);

	surface = TTF_RenderText_Solid(font13, name1, fontColor);
	rect.x = 50 + PLAYER_W;
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

	// Player 2
	rect.x = screen->clip_rect.w - PLAYER_W - 40;
	rect.y = 40;
	SDL_BlitSurface(character_sprites->at(select2), clip_right, screen, &rect);

	surface = TTF_RenderText_Solid(font13, name2, fontColor);
	rect.x = screen->clip_rect.w - PLAYER_W - surface->clip_rect.w - 50;
	rect.y = 40 + ((PLAYER_H - surface->clip_rect.h)/ 2);
	SDL_BlitSurface(surface, NULL, screen, &rect);
	SDL_FreeSurface(surface);

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

void CharacterSelect::load_sprites() {
	SDL_Surface * loaded;
	SDL_Surface * sprites;
	Uint32 colorkey;

	character_sprites = new std::vector<SDL_Surface*>(0);

	for(int idx = 0; idx < Battle::CHARACTER_COUNT; idx++) {
		loaded = SDL_LoadBMP(Battle::characters[idx].filename);
		sprites = SDL_DisplayFormat(loaded);
		SDL_FreeSurface(loaded);
		colorkey = SDL_MapRGB(sprites->format, 0, 255, 255);
		SDL_SetColorKey(sprites, SDL_SRCCOLORKEY, colorkey);
		
		character_sprites->push_back(sprites);
	}

	clip_avatar = new SDL_Rect();
	clip_avatar->x = 220;
	clip_avatar->y = 0;
	clip_avatar->w = 44;
	clip_avatar->h = 44;
	
	clip_left = new SDL_Rect();
	clip_left->x = 0;
	clip_left->y = 0;
	clip_left->w = PLAYER_W;
	clip_left->h = PLAYER_H;
	
	clip_right = new SDL_Rect();
	clip_right->x = 0;
	clip_right->y = PLAYER_H;
	clip_right->w = PLAYER_W;
	clip_right->h = PLAYER_H;
}

void CharacterSelect::free_sprites() {
	for(unsigned int idx = 0; idx < character_sprites->size(); idx++) {
		SDL_FreeSurface(character_sprites->at(idx));
		character_sprites->erase(character_sprites->begin() + idx);
	}
	delete character_sprites;
}
