#include "SDL/SDL.h"

#include <vector>

#include "Main.h"
#include "Timer.h"
#include "AudioController.h"

#include "Color.h"

#include "Gameplay.h"

#include "MissionSelect.h"
#include "Mission.h"

#include "NPC.h"
#include "ChickNPC.h"

#include "CharacterSelect.h"
#include "LevelSelect.h"
#include "LocalMultiplayer.h"
#include "LocalMultiplayerRoundEnd.h"

#include "Options.h"

#include "PlayerAnimation.h"

#include "Menu.h"

#include "ClientSettings.h"

#define MENU_TOP_OFFSET 180
#define MENU_ITEM_HEIGHT TILE_H
#define MENU_ITEM_WIDTH 128

#define DIRECTION_NONE	0
#define DIRECTION_LEFT	1
#define DIRECTION_RIGHT	2
#define DIRECTION_UP	4
#define DIRECTION_DOWN	8

const int Menu::ITEMCOUNT = /*5*/ 4;
const char * Menu::item[ITEMCOUNT] = {/*"MISSIONS", */"PLAY LOCAL", "PLAY ONLINE", "OPTIONS", "QUIT"};

Menu::Menu(Main &main) : SimpleDrawable(main), main_(main) {
}

Menu::~Menu() {
}

void Menu::run() {
	SDL_Event event;

	init();

	for (int i = 0; i < 4; i++) {
		input[i] = main_.input[i];
		input[i]->set_delay();
	}

	main_.audio->play_music(MUSIC_TITLE);

	frame = 0;

	started = false;
	input_master = main_.input_master;

	main_.autoreset = true;

	while (main_.running) {
		while (SDL_PollEvent(&event)) {
			main_.handle_event(&event);
			for (int i = 0; i < 4; i++) {
				input[i]->handle_event(&event);
			}
		}
		process_cursor();

		process_playeranimation();
		playeranimation->move();

		draw();

		main_.flip();
		frame++;

		if (!main_.running && main_.is_reset) {
			main_.running = true;
			main_.is_reset = false;

			if (input_master != NULL) {
				input_master->reset();
				input_master->set_delay();
			}

			started = false;
		}
	}
	main_.audio->stop_music();

	cleanup();
}

void Menu::draw_impl() {
	int i;
	SDL_Surface * text;
	SDL_Rect rect, rect_s;
	SDL_Surface * screen;

	screen = main_.screen;

	SDL_BlitSurface(main_.graphics->bg_menu, NULL, screen, NULL);

	rect.x = (WINDOW_WIDTH - title->w) / 2;
	rect.y = 40;
	SDL_BlitSurface(title, NULL, screen, &rect);

#ifndef PBWEB
	// Tile border
	rect_s.x = 0;
	rect_s.y = 0;
	rect_s.w = TILE_W;
	rect_s.h = TILE_H;

	rect.x = ((WINDOW_WIDTH - MENU_ITEM_WIDTH) / 2) - (TILE_W * 2);
	rect.y = MENU_TOP_OFFSET - TILE_H;
	for (i = 0; i < (MENU_ITEM_WIDTH / TILE_W) + 4; i++) {
		SDL_BlitSurface(main_.graphics->tiles, &rect_s, screen, &rect);
		rect.x += TILE_W;
	}

	for (i = 0; i < ITEMCOUNT; i++) {
		rect.x = surf_items_clip->at(i)->x - (TILE_W * 2);
		rect.y = surf_items_clip->at(i)->y - 8;
		SDL_BlitSurface(main_.graphics->tiles, &rect_s, screen, &rect);
		rect.x = surf_items_clip->at(i)->x + MENU_ITEM_WIDTH + TILE_W;
		SDL_BlitSurface(main_.graphics->tiles, &rect_s, screen, &rect);
	}

	rect.x = ((WINDOW_WIDTH - MENU_ITEM_WIDTH) / 2) - (TILE_W * 2);
	rect.y = MENU_TOP_OFFSET + (ITEMCOUNT * MENU_ITEM_HEIGHT);
	for (i = 0; i < (MENU_ITEM_WIDTH / TILE_W) + 4; i++) {
		SDL_BlitSurface(main_.graphics->tiles, &rect_s, screen, &rect);
		rect.x += TILE_W;
	}
#endif

	if (started) {
		// The menu
		for (i = 0; i < ITEMCOUNT; i++) {
			text = surf_items->at(i);

			if (selected_item == i) {
				rect.x = surf_items_clip->at(i)->x - TILE_W;
				rect.y = surf_items_clip->at(i)->y - 8;
				rect.w = MENU_ITEM_WIDTH + (TILE_W * 2);
				rect.h = MENU_ITEM_HEIGHT;

				SDL_FillRectColor(screen, &rect, MENU_CURSOR_COLOR);
			}

			SDL_BlitSurface(text, NULL, screen, surf_items_clip->at(i));
		}
	} else {
		// Press start
		if (!(frame & 0x20) || !(frame & 0x8)) {
			rect.x = (WINDOW_WIDTH - main_.graphics->text_pressstart->w) / 2;
			rect.y = ((WINDOW_HEIGHT - main_.graphics->text_pressstart->h) / 2) - 16;
			SDL_BlitSurface(main_.graphics->text_pressstart, NULL, screen, &rect);
		}
	}

	// Player animation
	playeranimation->draw(screen);

	// Player name
	if (draw_playername) {
		SDL_BlitSurface(main_.graphics->playername->at(playeranimation->character), 0, screen, &pos_playername);
	}

    // Credits
    unsigned int currect_credit = (frame / 180 % (credits_title.size() + 2));

    if (currect_credit < credits_title.size()) {
        rect.x = (WINDOW_WIDTH - credits_title.at(currect_credit)->w) / 2;
        rect.y = WINDOW_HEIGHT - credits_title.at(currect_credit)->h - 24;
        SDL_BlitSurface(credits_title.at(currect_credit), NULL, screen, &rect);

        if (currect_credit < credits_name.size()) {
            rect.x = (WINDOW_WIDTH - credits_name.at(currect_credit)->w) / 2;
            rect.y = WINDOW_HEIGHT - credits_name.at(currect_credit)->h - 4;
            SDL_BlitSurface(credits_name.at(currect_credit), NULL, screen, &rect);
        }
    }
}

void Menu::process_cursor() {
	if (started) {
		if (input_master->is_pressed(A_BACK)) {
			main_.running = false;
			main_.is_reset = true;
		}
		if (input_master->is_pressed(A_RUN) || input_master->is_pressed(A_JUMP) ||
			input_master->is_pressed(A_SHOOT) || input_master->is_pressed(A_BOMB) ||
			input_master->is_pressed(A_START)) {
			if (!(input_master->is_pressed(A_JUMP) && input_master->is_pressed(A_UP))) // It's likely that up and jump are the same keybind
				select();
		}

		if (input_master->is_pressed(A_UP))
			select_up();
		if (input_master->is_pressed(A_DOWN))
			select_down();
	} else {
		for (int i = 0; i < 4; i++) {
			if (input[i]->is_pressed(A_RUN) || input[i]->is_pressed(A_JUMP) ||
				input[i]->is_pressed(A_SHOOT) || input[i]->is_pressed(A_BOMB) ||
				input[i]->is_pressed(A_START)) {
				main_.audio->play(SND_SELECT);
				started = true;

				main_.input_master = input[i];
				input_master = input[i];

				if (main_.menu_skips_to_local_multiplayer) {
					start_local_multiplayer();
					started = false;
				}
			}
		}
	}
}

void Menu::select() {
	main_.audio->play(SND_SELECT);
	switch (selected_item + 1) {
		case 0:
			start_missions();
			break;
		case 1:
			start_local_multiplayer();
			break;
		case 2:
		{
			ClientSettings clset(main_);
			clset.run();
			break;
		}
		case 3:
		{
			Options options(main_);
			options.run();
			break;
		}
		case 4:
			SDL_Delay(500);
			main_.running = false;
			break;
	}
}

void Menu::start_missions() {
	MissionSelect * ms;
	Mission * m;
	Level * level;
	Player * player;

	bool running;

	running = true;

	player = new Player(0, 1, main_);
	player->input = input_master;
	player->bullets = 0;
	player->bombs = 0;

	ms = new MissionSelect(main_);

	while (main_.running && running && !ms->cancel) {
		ms->run();

		if (main_.running && running && !ms->cancel) {
			level = new Level(main_);
			level->load(Mission::MISSIONS[ms->mission].filename);

			m = new Mission(main_);
			m->add_player(player);
			m->set_level(level);
			m->run();

			delete level;
			delete m;
		}
	}

	delete ms;
	delete player;

	main_.audio->play_music(MUSIC_TITLE);
}

void Menu::start_local_multiplayer() {
	main_.draw_loading_screen();

	Player ** player;

	Level * level;
	LocalMultiplayer * lmp;
	LocalMultiplayerRoundEnd * end;
	CharacterSelect * cs;
	LevelSelect * ls;

	int round;

	int winner;
	int highest_score;

	bool running;
	bool change_character;
	bool change_level;

	player = new Player*[4];

	level = NULL;
	for (int i = 0; i < 4; i++) {
		player[i] = new Player(0, (i + 1), main_);
		player[i]->input = input[i];
	}

	running = true;
	round = 0;

	cs = new CharacterSelect(main_);
	ls = new LevelSelect(main_);

	change_level = true;
	change_character = true;

	while (main_.running && running) {
		round++;

		if (change_character) {
			main_.draw_loading_screen();
			cs->run();

			if (cs->cancel) break;

			for (int i = 0; i < 4; i++) {
				if (cs->player_joined[i])
					player[i]->set_character(cs->player_select[i]);
			}
		}

		for (int i = 0; i < 4; i++) {
			if (cs->player_joined[i])
				player[i]->reset();
		}

		if (change_level) {
			main_.draw_loading_screen();
			ls->run();

			if (ls->cancel) break;

			if (level != NULL)
				delete level;

			level = new Level(main_);
			level->load(Level::LEVELS[ls->level].filename);
		}

		lmp = new LocalMultiplayer(main_);
		lmp->set_level(level);

		for (int i = 0; i < 4; i++) {
			if (cs->player_joined[i])
				lmp->add_player(player[i]);
		}
		lmp->run();

		// This is duplicate logic with.. search for "@MARK:1", the code is different
		highest_score = 0;
		winner = -1;
		for (int i = 0; i < 4; i++) {
			if (cs->player_joined[i]) {
				if (player[i]->score == highest_score) {
					winner = -1;
				}
				if (player[i]->score > highest_score) {
					winner = i;
					highest_score = player[i]->score;
				}
			}
		}

		if (winner != -1) {
			player[winner]->rounds_won++;
		}

		end = new LocalMultiplayerRoundEnd(main_);
		for (int i = 0; i < 4; i++) {
			if (cs->player_joined[i]) {
				end->add_player(player[i]);
			}
		}
		if (winner != -1)
			end->winner = player[winner];
		else
			end->winner = NULL;
		end->round = round;
		end->run();
		if (end->result == ROUNDEND_CHANGE_CHARACTER) {
			change_level = true;
			change_character = true;
		}
		if (end->result == ROUNDEND_CHANGE_LEVEL) {
			change_character = false;
			change_level = true;
		}
		if (end->result == ROUNDEND_QUIT)
			running = false;
		delete end;

		delete lmp;
	}

	delete cs;
	delete ls;
	if (level != NULL)
		delete level;
	for (int i = 0; i < 4; i++) {
		if (player[i] != NULL)
			delete player[i];
	}
	delete player;

	main_.audio->play_music(MUSIC_TITLE);
}

void Menu::process_playeranimation() {
	if (playeranimation->position->x < -PLAYER_W)
		next_playeranimation();
	if (frame - animation_start == 0) {
		playeranimation->position->x = WINDOW_WIDTH;
		playeranimation->position->y = (TILE_H * 13) - PLAYER_H;
		playeranimation->is_walking = true;
		playeranimation->is_running = true;
		playeranimation->momentumx = -20;
		playeranimation->animate_in_place = false;
		playeranimation->direction = -1;
	}
	if (frame - animation_start == 41) {
		playeranimation->direction = 1;
	}
	if (playeranimation->momentumx == 0) {
		playeranimation->is_walking = false;
		playeranimation->direction = -1;
	}
	if (frame - animation_start == 150) {
		playeranimation->is_walking = true;
		playeranimation->direction = -1;
	}
	if (!draw_playername && playeranimation->momentumx == 0) {
		draw_playername = true;
		pos_playername.x = (playeranimation->position->x + (playeranimation->position->w / 2)) - (main_.graphics->playername->at(playeranimation->character)->w / 2);
		pos_playername.y = playeranimation->position->y - 20;
	}
	if (draw_playername && playeranimation->momentumx != 0) {
		draw_playername = false;
	}
}

void Menu::next_playeranimation() {
	int character;
	character = playeranimation->character;
	delete playeranimation;

	playeranimation = new PlayerAnimation((character + 1) % Player::CHARACTER_COUNT, main_);
	animation_start = frame;
}

void Menu::select_up() {
	main_.audio->play(SND_SELECT);

	selected_item--;

	if (selected_item < 0) {
		selected_item = ITEMCOUNT - 1;
	}
}

void Menu::select_down() {
	main_.audio->play(SND_SELECT);

	selected_item++;

	if (selected_item == ITEMCOUNT) {
		selected_item = 0;
	}
}

void Menu::init() {
	SDL_Surface * surface;
	SDL_Rect * rect;

	selected_item = 0;

#ifdef PBWEB
	title = main_.text->render_text_large_shadow("PB WEB   MEDIA ");
#elif defined(TWEAKERS)
	title = main_.text->render_text_large_shadow("TWEAK   BATTLE");
#else
	title = main_.text->render_text_large_shadow("SMASH BATTLE");
#endif

	surf_items = new std::vector<SDL_Surface*>(0);
	surf_items_clip = new std::vector<SDL_Rect*>(0);
	for (int i = 0; i < ITEMCOUNT; i++) {
		surface = main_.text->render_text_medium(item[i]);
		surf_items->push_back(surface);

		rect = new SDL_Rect();
		rect->x = (WINDOW_WIDTH - MENU_ITEM_WIDTH) / 2;
#ifdef PBWEB
		rect->y = MENU_TOP_OFFSET + (i * MENU_ITEM_HEIGHT) - 12;
#else
		rect->y = MENU_TOP_OFFSET + (i * MENU_ITEM_HEIGHT) + 8;
#endif
		surf_items_clip->push_back(rect);
	}

    credits_title.push_back(main_.text->render_text_medium_gray("CODE / CONCEPT"));
    credits_name.push_back(main_.text->render_text_medium("BERT HEKMAN"));
    credits_title.push_back(main_.text->render_text_medium_gray("GRAPHICS / CONCEPT"));
    credits_name.push_back(main_.text->render_text_medium("JEROEN GROENEWEG"));
    credits_title.push_back(main_.text->render_text_medium_gray("MUSIC"));
    credits_name.push_back(main_.text->render_text_medium("NICK PERRIN"));
    credits_title.push_back(main_.text->render_text_medium_gray("NETWORKING CODE"));
    credits_name.push_back(main_.text->render_text_medium("RAY BURGEMEESTRE"));
    credits_title.push_back(main_.text->render_text_medium_gray("ADDITIONAL GRAPHICS"));
    credits_name.push_back(main_.text->render_text_medium("JEROEN STENGS"));
    credits_title.push_back(main_.text->render_text_medium_gray("ADDITIONAL CODE"));
    credits_name.push_back(main_.text->render_text_medium("JORDY MOOS"));
    credits_title.push_back(main_.text->render_text_medium_gray("ADDITIONAL CODE"));
    credits_name.push_back(main_.text->render_text_medium("TIM VAN DENSEN"));

	playeranimation = new PlayerAnimation(0, main_);
	animation_start = 0;
}

void Menu::cleanup() {
	SDL_FreeSurface(title);

	for (unsigned int i = 0; i < surf_items->size(); i++) {
		SDL_FreeSurface(surf_items->at(i));
	}
	surf_items->clear();
	delete surf_items;

	for (unsigned int i = 0; i < surf_items_clip->size(); i++) {
		delete surf_items_clip->at(i);
	}
	surf_items_clip->clear();
	delete surf_items_clip;

	delete playeranimation;
}
