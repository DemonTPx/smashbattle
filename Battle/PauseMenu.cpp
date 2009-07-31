#include "SDL/SDL.h"

#include <vector>

#include "Gameplay.h"
#include "Player.h"
#include "PauseMenu.h"

PauseMenu::PauseMenu(SDL_Surface * s) {
	screen = s;
	options = new std::vector<char*>(0);
	selected_option = 0;
	draw_width = 160;
}

PauseMenu::~PauseMenu() {
	options->clear();
	delete options;
}

void PauseMenu::add_option(char * name) {
	options->push_back(name);
}

int PauseMenu::pause(Player * p) {
	SDL_Event event;
	
	Main::audio->play(SND_PAUSE);
	Main::audio->pause_music();

	player = p;
	paused = true;

	selected_option = 0;

	player->input->set_delay();
	player->input->reset();

	while(Main::running && paused) {
		while(SDL_PollEvent(&event)) {
			Main::instance->handle_event(&event);

			// Handle player input
			for(unsigned int idx = 0; idx < Gameplay::instance->players->size(); idx++) {
				Player * p = Gameplay::instance->players->at(idx);
				p->input->handle_event(&event);
			}
		}

		process();

		draw();

		Main::instance->flip();
	}

	player->input->unset_delay();

	Main::audio->unpause_music();
	return selected_option;
}

void PauseMenu::process() {
	if(player->input->is_pressed(A_DOWN)) {
		Main::audio->play(SND_SELECT);
		selected_option = (selected_option + 1) % (int)options->size();
	}

	if(player->input->is_pressed(A_UP)) {
		Main::audio->play(SND_SELECT);
		if(selected_option == 0) selected_option += (int)options->size();
		selected_option = selected_option - 1;
	}

	if(player->input->is_pressed(A_START)) {
		Main::audio->play(SND_PAUSE);
		selected_option = 0;
		paused = false;
	}

	if(player->input->is_pressed(A_JUMP) || player->input->is_pressed(A_RUN) ||
		player->input->is_pressed(A_SHOOT) || player->input->is_pressed(A_BOMB)) {
		if(!(player->input->is_pressed(A_JUMP) && player->input->is_pressed(A_UP))) { // It's likely that up and jump are the same keybind
			Main::audio->play(SND_SELECT);
			paused = false;
		}
	}
}

void PauseMenu::draw() {
	SDL_Surface * surface;
	SDL_Rect rect;
	Uint32 color;

	int width, height;

	width = draw_width;
	
	color = Player::COLORS[player->number - 1];

	height = (20 * (int)options->size()) + 44;

	rect.x = (screen->w - width) / 2;
	rect.y = (screen->h  - height) / 2;
	rect.w = width;
	rect.h = height;

	SDL_FillRect(screen, &rect, color);

	rect.x += 2;
	rect.y += 2;
	rect.w -= 4;
	rect.h -= 4;

	SDL_FillRect(screen, &rect, 0);

	surface = Main::text->render_text_medium("PAUSE");
	rect.x = (screen->w - surface->w) / 2;
	rect.y += 8;

	SDL_BlitSurface(surface, NULL, screen, &rect);
	SDL_FreeSurface(surface);

	rect.y += 8;

	for(unsigned int i = 0; i < options->size(); i++) {
		char * text;
		text = options->at(i);
		
		rect.x = (screen->w - (width - 12)) / 2;
		rect.y += 20;
		rect.w = width - 12;
		rect.h = 22;

		if(selected_option == i) {
			rect.y -= 4;
			SDL_FillRect(screen, &rect, color);
			rect.y += 4;
		}

		surface = Main::text->render_text_medium(text);
		rect.x = (screen->w - surface->w) / 2 ;
		SDL_BlitSurface(surface, NULL, screen, &rect);
		SDL_FreeSurface(surface);

	}
}
