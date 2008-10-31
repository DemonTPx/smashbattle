#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"

#include "Timer.h"
#include "AudioController.h"
#include "Main.h"
#include "Battle.h"

#include "Menu.h"

#define MENU_TOP_OFFSET 200
#define MENU_ITEM_HEIGHT 26

const char * Menu::item[10] = {"START", "OPTIONS", "QUIT"};
const int Menu::itemcount = 3;

Menu::Menu() {
	font26 = TTF_OpenFont("fonts/slick.ttf", 26);
	font13 = TTF_OpenFont("fonts/slick.ttf", 13);
	fontColor.r = 255;
	fontColor.g = 255;
	fontColor.b = 255;

	selected_item = 0;
}

Menu::~Menu() {
	TTF_CloseFont(font26);
	TTF_CloseFont(font13);
}

void Menu::run() {
	SDL_Event event;

	SDL_Surface * surface;
	surface = SDL_LoadBMP("gfx/title.bmp");
	bg = SDL_DisplayFormat(surface);
	SDL_FreeSurface(surface);

	draw();
	
	Main::audio->play_music(MUSIC_TITLE);

	while (Main::running) {
		while(SDL_PollEvent(&event)) {
			Main::instance->handle_event(&event);
			if(event.type == SDL_KEYDOWN) {
				switch(event.key.keysym.sym) {
					case SDLK_UP:
						Main::audio->play(SND_SELECT);
						selected_item--;
						if(selected_item < 0) selected_item = itemcount - 1;
						draw();
						break;
					case SDLK_DOWN:
						Main::audio->play(SND_SELECT);
						selected_item++;
						if(selected_item == itemcount) selected_item = 0;
						draw();
						break;
					case SDLK_PAGEUP:
						Main::audio->play(SND_SELECT);
						selected_item = 0;
						draw();
						break;
					case SDLK_PAGEDOWN:
						Main::audio->play(SND_SELECT);
						selected_item = itemcount - 1;
						draw();
						break;
					case SDLK_RETURN:
						Main::audio->play(SND_SELECT);
						if(selected_item == 0) {
							Battle battle;
							battle.run();
							draw();
						}
						if(selected_item == 2) {
							SDL_Delay(500);
							Main::running = false;
						}
						Main::audio->play_music(MUSIC_TITLE);
						break;
					default:
						break;
				}
			}
			if(event.type == SDL_JOYBUTTONDOWN) {
				switch(event.jbutton.button) {
					case 0:
					case 1:
					case 2:
					case 3:
						Main::audio->play(SND_SELECT);
						if(selected_item == 0) {
							Battle battle;
							battle.run();
							draw();
							Main::audio->play_music(MUSIC_TITLE);
						}
						if(selected_item == 2) {
							SDL_Delay(500);
							Main::running = false;
						}
						break;
				}
			}
			if(event.type == SDL_JOYAXISMOTION) {
				if(event.jaxis.axis == 1) {
					if(event.jaxis.value < -6400) {
						Main::audio->play(SND_SELECT);
						selected_item--;
						if(selected_item < 0) selected_item = itemcount - 1;
						draw();
						break;
					}
					if(event.jaxis.value > 6400) {
						Main::audio->play(SND_SELECT);
						selected_item++;
						if(selected_item == itemcount) selected_item = 0;
						draw();
						break;
					}
				}
			}
		}
		Main::instance->flip();
	}
	Main::audio->stop_music();
}

void Menu::draw() {
	int i;
	SDL_Surface * text, * highlight;
	SDL_Rect rect;
	SDL_Surface * screen;

	screen = Main::instance->screen;

	//SDL_FillRect(screen, NULL, 0);
	SDL_BlitSurface(bg, NULL, screen, NULL);

	for(i = 0; i < itemcount; i++) {
		text = TTF_RenderText_Solid(font26, item[i], fontColor);
		
		if(selected_item == i) {
			highlight = SDL_CreateRGBSurface(NULL, text->w + 10, MENU_ITEM_HEIGHT, 32, 0, 0, 0, 0);
			SDL_FillRect(highlight, NULL, 0x444488);

			rect.x = ((screen->w - text->w) / 2) - 5;
			rect.y = MENU_TOP_OFFSET + (i * MENU_ITEM_HEIGHT) - 3;

			SDL_BlitSurface(highlight, NULL, screen, &rect);
			SDL_FreeSurface(highlight);
		}

		rect.x = (screen->w - text->w) / 2;
		rect.y = MENU_TOP_OFFSET + (i * MENU_ITEM_HEIGHT);

		SDL_BlitSurface(text, NULL, screen, &rect);
		SDL_FreeSurface(text);
	}
	
	char credit1[] = "Programming by Bert Hekman";
	char credit2[] = "Graphics by Jeroen Groeneweg";
	char credit3[] = "Music by Nick Perrin";

	text = TTF_RenderText_Solid(font13, credit1, fontColor);
	rect.x = (WINDOW_WIDTH - text->w) / 2;
	rect.y = WINDOW_HEIGHT - 35;
	SDL_BlitSurface(text, NULL, screen, &rect);
	SDL_FreeSurface(text);
	
	text = TTF_RenderText_Solid(font13, credit2, fontColor);
	rect.x = (WINDOW_WIDTH - text->w) / 2;
	rect.y = WINDOW_HEIGHT - 25;
	SDL_BlitSurface(text, NULL, screen, &rect);
	SDL_FreeSurface(text);

	text = TTF_RenderText_Solid(font13, credit3, fontColor);
	rect.x = (WINDOW_WIDTH - text->w) / 2;
	rect.y = WINDOW_HEIGHT - 15;
	SDL_BlitSurface(text, NULL, screen, &rect);
	SDL_FreeSurface(text);
}
