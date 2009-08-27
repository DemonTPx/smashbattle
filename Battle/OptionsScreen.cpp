#include "SDL/SDL.h"

#include <vector>

#include "OptionsScreen.h"

#define DIRECTION_NONE	0
#define DIRECTION_LEFT	1
#define DIRECTION_RIGHT	2
#define DIRECTION_UP	4
#define DIRECTION_DOWN	8

OptionsScreen::OptionsScreen() {
	items = new std::vector<OptionItem*>(0);

	align = LEFT;

	menu_item_height = 26;
	menu_top_offset = 30;
	menu_left_offset = 20;
	menu_options_left_offset = 250;
}

void OptionsScreen::run() {
	SDL_Event event;

	init();

	frame = 0;

	running = true;

	input = Main::instance->input_master;
	input->set_delay();
	input->reset();

	while (Main::running && running) {
		while(SDL_PollEvent(&event)) {
			Main::instance->handle_event(&event);

			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
				running = false;
				break;
			}

			input->handle_event(&event);
		}
		process_cursor();
		
		draw();

		Main::instance->flip();
		frame++;
	}

	cleanup();
}

void OptionsScreen::draw() {
	unsigned int i, j;
	SDL_Surface * text;
	SDL_Rect rect;
	SDL_Surface * screen;
	OptionItem * item;

	screen = Main::instance->screen;

	SDL_BlitSurface(background, NULL, screen, NULL);

	for(i = 0; i < items->size(); i++) {
		item = items->at(i);
		text = item->surf_name;
		
		if(selected_item == (int)i) {
			rect.x = item->rect_name->x - 5;
			rect.y = item->rect_name->y - 5;
			rect.w = text->w + 10;
			rect.h = menu_item_height;
			SDL_FillRect(screen, &rect, 0x444488);
		}

		SDL_BlitSurface(text, NULL, screen, item->rect_name);

		if(item->options != NULL) {
			for(j = 0; j < item->options->size(); j++) {
				text = item->surf_options->at(j);
				if(item->selected == j) {
					rect.x = item->rect_options->at(j)->x - 5;
					rect.y = item->rect_options->at(j)->y - 5;
					rect.w = text->w + 10;
					rect.h = menu_item_height;
					SDL_FillRect(screen, &rect, 0x444488);
				}

				SDL_BlitSurface(text, NULL, screen, item->rect_options->at(j));
			}
		}
	}
}

void OptionsScreen::process_cursor() {
	if(input->is_pressed(A_RUN) || input->is_pressed(A_JUMP) ||
			input->is_pressed(A_SHOOT) || input->is_pressed(A_BOMB)) {
		if(!(input->is_pressed(A_JUMP) && input->is_pressed(A_UP))) // It's likely that up and jump are the same keybind
			select();
	}

	if(input->is_pressed(A_LEFT))
		select_left();
	if(input->is_pressed(A_RIGHT))
		select_right();
	if(input->is_pressed(A_UP))
		select_up();
	if(input->is_pressed(A_DOWN))
		select_down();
}

void OptionsScreen::select() {
	Main::audio->play(SND_SELECT);
	item_selected();
}

void OptionsScreen::select_up() {
	Main::audio->play(SND_SELECT);

	selected_item--;

	if(selected_item < 0) {
		selected_item = (int)items->size() - 1;
	}
}

void OptionsScreen::select_down() {
	Main::audio->play(SND_SELECT);

	selected_item++;

	if(selected_item == (int)items->size()) {
		selected_item = 0;
	}
}

void OptionsScreen::select_left() {
	OptionItem * item;
	unsigned int count;

	item = items->at(selected_item);
	if(item->options == NULL) return;
	
	count = (unsigned int)item->options->size();

	if(item->selected == 0)
		item->selected = count - 1;
	else
		item->selected--;

	selection_changed();
}

void OptionsScreen::select_right() {
	OptionItem * item;
	unsigned int count;

	item = items->at(selected_item);
	if(item->options == NULL) return;
	
	count = (unsigned int)item->options->size();

	if(item->selected == (count - 1))
		item->selected = 0;
	else
		item->selected++;
	
	selection_changed();
}

void OptionsScreen::add_item(OptionItem * item) {
	items->push_back(item);
}

void OptionsScreen::item_selected() { }
void OptionsScreen::selection_changed() { }

void OptionsScreen::init() {
	SDL_Surface * surface;
	SDL_Rect * rect;
	SDL_Rect rect_d;
	OptionItem * item;
	int x;
	
	background = SDL_CreateRGBSurface(0, WINDOW_WIDTH, WINDOW_HEIGHT, 32, 0, 0, 0, 0);

	for(int y = 0; y < WINDOW_HEIGHT; y += Main::graphics->bg_grey->h) {
		for(int x = 0; x < WINDOW_WIDTH; x += Main::graphics->bg_grey->w) {
			rect_d.x = x;
			rect_d.y = y;
			SDL_BlitSurface(Main::graphics->bg_grey, NULL, background, &rect_d);
		}
	}

	if(selected_item < 0 || selected_item >= (int)items->size())
		selected_item = 0;

	screen_w = Main::instance->screen->w;
	screen_h = Main::instance->screen->h;

	for(unsigned int i = 0; i < items->size(); i++) {
		item = items->at(i);
		
		item->surf_name = Main::text->render_text_medium(item->name);

		item->rect_name = new SDL_Rect();

		item->rect_name->y = menu_top_offset + (i * menu_item_height) - 3;
		if(align == LEFT) {
			item->rect_name->x = menu_left_offset;
		} else if(align == CENTER) {
			item->rect_name->x = (screen_w - item->surf_name->w) / 2;
		} else {
			item->rect_name->x = screen_w - item->surf_name->w - menu_left_offset;
		}

		x = menu_options_left_offset;

		if(item->options != NULL) {
			item->surf_options = new std::vector<SDL_Surface *>(0);
			item->rect_options = new std::vector<SDL_Rect *>(0);
			for(unsigned int j = 0; j < item->options->size(); j++) {
				surface = Main::text->render_text_medium(item->options->at(j));
				item->surf_options->push_back(surface);

				rect = new SDL_Rect();
				rect->x = x;
				rect->y = menu_top_offset + (i * menu_item_height) - 3;

				item->rect_options->push_back(rect);

				x += surface->w + 10;
			}
		} else {
			item->surf_options = NULL;
			item->rect_options = NULL;
		}
	}
}

void OptionsScreen::cleanup() {
	for(unsigned int i = 0; i < items->size(); i++) {
		if(items->at(i)->options != NULL) {
			for(unsigned int j = 0; j < items->at(i)->options->size(); j++) {
				SDL_FreeSurface(items->at(i)->surf_options->at(j));
				delete(items->at(i)->rect_options->at(j));
			}
			
			delete items->at(i)->surf_options;
			delete items->at(i)->rect_options;

			items->at(i)->options->clear();
			delete items->at(i)->options;
		}
		SDL_FreeSurface(items->at(i)->surf_name);
		delete items->at(i)->rect_name;

		delete items->at(i);
	}
	items->clear();
	delete items;

	SDL_FreeSurface(background);
}
