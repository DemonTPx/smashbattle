#include "SDL/SDL.h"

#include "Main.h"
#include "Player.h"
#include "Level.h"
#include "LocalMultiplayerRoundEnd.h"
#include "Color.h"

#define MENU_TOP_OFFSET 368
#define MENU_ITEM_HEIGHT TILE_H
#define MENU_ITEM_WIDTH 224

#define DIRECTION_NONE	0
#define DIRECTION_LEFT	1
#define DIRECTION_RIGHT	2
#define DIRECTION_UP	4
#define DIRECTION_DOWN	8

#define BEGIN_DELAY 60

#ifndef WIN32
#define sprintf_s snprintf
#endif

const int LocalMultiplayerRoundEnd::ITEMCOUNT = 3;
const char * LocalMultiplayerRoundEnd::item[ITEMCOUNT] = {"CHANGE LEVEL", "CHANGE CHARACTER", "END GAME"};

LocalMultiplayerRoundEnd::LocalMultiplayerRoundEnd(Main &main): SimpleDrawable(main), main_(main) {
	players = new std::vector<Player *>();
}

LocalMultiplayerRoundEnd::~LocalMultiplayerRoundEnd() {
	delete players;
}

void LocalMultiplayerRoundEnd::run() {
	SDL_Event event;

	init();

	main_.audio->play_music(MUSIC_END);
	
	ready = false;
	frame = 0;

	reset_input();

	Player * p;

	while (main_.running && !ready) {
		while(SDL_PollEvent(&event)) {
			main_.handle_event(&event);
			
			handle_event(*input, event);
		}

		do_run();

		process_cursor();

		frame++;
		draw();

		main_.flip();
	}

	cleanup();
}

void LocalMultiplayerRoundEnd::reset_input()
{
	input = main_.input_master;
	input->set_delay();
	input->reset();
}

void LocalMultiplayerRoundEnd::handle_event(GameInput &input, SDL_Event &event)
{
	input.handle_event(&event);
}

void LocalMultiplayerRoundEnd::init() {
	SDL_Surface * surface;
	SDL_Surface * text;
	SDL_Rect * clip;
	SDL_Rect rect, rect_s;
	SDL_Rect r_block;
	double accuracy;
	char str[30];
	short temp;
	bool swapped;
	int last_score;
	int cup;
	Player * pl;

	selected_item = 0;

	// Menu
	surf_items = new std::vector<SDL_Surface*>(0);
	surf_items_clip = new std::vector<SDL_Rect*>(0);
	for(int i = 0; i < ITEMCOUNT; i++) {
		surface = main_.text->render_text_medium(item[i]);
		surf_items->push_back(surface);

		clip = new SDL_Rect();
		clip->x = (WINDOW_WIDTH - MENU_ITEM_WIDTH) / 2;
		clip->y = MENU_TOP_OFFSET + (i * MENU_ITEM_HEIGHT) + 6;
		surf_items_clip->push_back(clip);
	}

	// Result
	if(winner == NULL) {
		surf_result = main_.text->render_text_large("DRAW");
	} else {
		sprintf_s(str, 30, "%s WINS", winner->name);
		surf_result = main_.text->render_text_large(str);
	}

	// Determine player positions
	for(unsigned int i = 0; i < players->size(); i++) {
		order[i] = i;
	}
	do {
		swapped = false;
		for(unsigned short i = 0; i < players->size() - 1; i++) {
			if(players->at(order[i])->score < players->at(order[i + 1])->score) {
				temp = order[i];
				order[i] = order[i + 1];
				order[i + 1] = temp;
				swapped = true;
			}
		}
	} while(swapped);

	// Prerender
	
	background = SDL_CreateRGBSurface(0, WINDOW_WIDTH, WINDOW_HEIGHT, 32, 0, 0, 0, 0);

	for(int y = 0; y < WINDOW_HEIGHT; y += main_.graphics->bg_grey->h) {
		for(int x = 0; x < WINDOW_WIDTH; x += main_.graphics->bg_grey->w) {
			rect.x = x;
			rect.y = y;
			SDL_BlitSurface(main_.graphics->bg_grey, NULL, background, &rect);
		}
	}

	// Player statistics
	r_block.x = 0;
	r_block.y = 0;
	r_block.w = WINDOW_WIDTH - 20;
	r_block.h = 72;

	last_score = -1;
	cup = 0;

	for(unsigned int i = 0; i < players->size(); i++) {
		pl = players->at(order[i]);

		r_block.x = 10;
		r_block.y = 50 + (76 * i);

		SDL_FillRectColor(background, &r_block, Player::COLORS[ players->at(order[i])->suit_number]);

		rect.x = r_block.x + 4;
		rect.y = r_block.y + 4;
		rect.w = r_block.w - 8;
		rect.h = r_block.h - 8;
		SDL_FillRectColor(background, &rect, 0);

		// Player number
		rect.x = r_block.x + 72;
		rect.y = r_block.y + 10;
		sprintf_s(str, 5, "P%d", get_player_number(*(players->at(order[i]))));
		text = main_.text->render_text_large(str);
		SDL_BlitSurface(text, NULL, background, &rect);
		SDL_FreeSurface(text);

		// Player name
		rect.x = r_block.x + 72;
		rect.y = r_block.y + 48;
		text = main_.text->render_text_medium(pl->name);
		SDL_BlitSurface(text, NULL, background, &rect);
		SDL_FreeSurface(text);

		// Static avatars
		rect.x = r_block.x + 14;
		rect.y = r_block.y + 14;
		if(players->at(order[i]) != winner)
			SDL_BlitSurface(players->at(order[i])->sprites, main_.graphics->player_clip[SPR_AVATAR], background, &rect);

		// Cup
		if(cup < 3) {
			if(last_score == -1 || last_score > pl->score) {
				if(last_score != -1)
					cup++;
				last_score = pl->score;
			}
			rect_s.x = (CUP_W * cup);
			rect_s.y = 0;
			rect_s.w = CUP_W;
			rect_s.h = CUP_H;
			rect.x = r_block.x + 130;
			rect.y = r_block.y + 10;
			SDL_BlitSurface(main_.graphics->cups, &rect_s, background, &rect);
		}

		// Kills
		rect.x = r_block.x + 190;
		rect.y = r_block.y + 10;

		for (std::vector<Kill>::iterator k = pl->kill_list->begin(); k != pl->kill_list->end(); k++) {
			// Draw kill move
			SDL_Surface * s_kill_move = main_.graphics->kill_moves;
			SDL_Rect rect_km;
			SDL_Rect rect_km_d;

			rect_km.x = k->move * KILL_MOVE_W;
			rect_km.y = 0;
			rect_km.w = KILL_MOVE_W;
			rect_km.h = KILL_MOVE_H;

			rect_km_d.x = rect.x + ((PLAYER_W - rect_km.w) / 2);
			rect_km_d.y = rect.y + PLAYER_W + 5 + ((PLAYER_W - rect_km.h) / 2);
			rect_km_d.w = rect_km.w;
			rect_km_d.h = rect_km.h;

			SDL_BlitSurface(s_kill_move, &rect_km, background, &rect_km_d);

			// Draw killed player
			SDL_BlitSurface(k->victim->sprites, main_.graphics->player_clip[SPR_R_HEAD], background, &rect);
			rect.x += PLAYER_W + 2;
		}

		// Score
		sprintf_s(str, 4, "%d", pl->score);
		text = main_.text->render_text_large(str);
		rect.x = r_block.x + r_block.w - text->w - 22;
		rect.y = r_block.y + 10;
		SDL_BlitSurface(text, NULL, background, &rect);
		SDL_FreeSurface(text);

		// Rounds won
		sprintf_s(str, 20, "%d", pl->rounds_won);
		text = main_.text->render_text_medium_gray(str);
		rect.x = r_block.x + r_block.w - text->w - 22;
		rect.y = r_block.y + r_block.h - text->h - 10;
		SDL_BlitSurface(text, NULL, background, &rect);
		SDL_FreeSurface(text);
	}
}


int LocalMultiplayerRoundEnd::get_player_number(Player &player)
{
	return player.number;
}

void LocalMultiplayerRoundEnd::cleanup() {
	for(unsigned int i = 0; i < surf_items->size(); i++) {
		SDL_FreeSurface(surf_items->at(i));
	}
	delete surf_items;

	for(unsigned int i = 0; i < surf_items_clip->size(); i++) {
		delete surf_items_clip->at(i);
	}
	delete surf_items_clip;

	SDL_FreeSurface(surf_result);

	SDL_FreeSurface(background);
}

void LocalMultiplayerRoundEnd::draw_impl() {
	SDL_Rect rect;
	SDL_Surface * screen;

	screen = main_.screen;

	SDL_BlitSurface(background, NULL, screen, NULL);
	
	draw_menu();
	
	// RESULTS
	rect.x = (WINDOW_WIDTH - surf_result->w) / 2;
	rect.y = 10;
	if(!(frame & 0x20))
		SDL_BlitSurface(surf_result, NULL, screen, &rect);

	for(unsigned int i = 0; i < players->size(); i++) {
		rect.x = 24;
		rect.y = 64 + (76 * i);

		// Avatar
		if(players->at(order[i]) == winner) {
			if(frame & 0x10)
				SDL_BlitSurface(players->at(order[i])->sprites, main_.graphics->player_clip[SPR_AVATAR_SELECTED], screen, &rect);
			else
				SDL_BlitSurface(players->at(order[i])->sprites, main_.graphics->player_clip[SPR_AVATAR], screen, &rect);
		}
	}
}

void LocalMultiplayerRoundEnd::draw_menu()
{
	int i = 0;
	SDL_Surface * text = NULL;
	SDL_Surface * screen = main_.screen;
	SDL_Rect rect;

	if(frame >= BEGIN_DELAY) {
		for(int i = 0; i < ITEMCOUNT; i++) {

			text = surf_items->at(i);
			
			if(selected_item == i) {
				rect.x = surf_items_clip->at(i)->x - TILE_W;
				rect.y = surf_items_clip->at(i)->y - 8;
				rect.w = MENU_ITEM_WIDTH + (TILE_W * 2);
				rect.h = MENU_ITEM_HEIGHT;

				SDL_FillRectColor(screen, &rect, MENU_CURSOR_COLOR);
			}

			SDL_BlitSurface(text, NULL, screen, surf_items_clip->at(i));
		}
	}
}

void LocalMultiplayerRoundEnd::process_cursor() {
	if(frame < BEGIN_DELAY)
		return;

	if (input->is_pressed(A_BACK)) {
		result = ROUNDEND_QUIT;
		ready = true;
	}

	if(input->is_pressed(A_RUN) || input->is_pressed(A_JUMP) ||
			input->is_pressed(A_SHOOT) || input->is_pressed(A_BOMB) ||
			input->is_pressed(A_START)) {
		if(!(input->is_pressed(A_JUMP) && input->is_pressed(A_UP))) { // It's likely that up and jump are the same keybind
			select();
		}
	}

	if(input->is_pressed(A_UP))
		select_up();
	if(input->is_pressed(A_DOWN))
		select_down();
}

void LocalMultiplayerRoundEnd::select() {
	main_.audio->play(SND_SELECT);
	switch(selected_item) {
		case 0:
			result = ROUNDEND_CHANGE_LEVEL;
			ready = true;
			break;
		case 1:
			result = ROUNDEND_CHANGE_CHARACTER;
			ready = true;
			break;
		case 2:
			result = ROUNDEND_QUIT;
			ready = true;
			break;
	}
	main_.audio->play_music(MUSIC_TITLE);
}

void LocalMultiplayerRoundEnd::select_up() {
	main_.audio->play(SND_SELECT);

	selected_item--;

	if(selected_item < 0) {
		selected_item = ITEMCOUNT - 1;
	}
}

void LocalMultiplayerRoundEnd::select_down() {
	main_.audio->play(SND_SELECT);

	selected_item++;

	if(selected_item == ITEMCOUNT) {
		selected_item = 0;
	}
}

void LocalMultiplayerRoundEnd::add_player(Player *p) {
	players->push_back(p);
}
