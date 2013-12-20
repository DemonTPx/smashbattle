#include "SDL/SDL.h"

#include <vector>

#include "Main.h"
#include "Timer.h"
#include "AudioController.h"

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

#include <algorithm>
#include "ServerListing.h"
#include "rest/Token.h"
#include "rest/ServerList.h"
#include "util/json.h"
#include "util/stringutils.hpp"
#include "util/sha256.h"

#define MENU_TOP_OFFSET 180
#define MENU_ITEM_HEIGHT TILE_H
#define MENU_ITEM_WIDTH 128

#define DIRECTION_NONE	0
#define DIRECTION_LEFT	1
#define DIRECTION_RIGHT	2
#define DIRECTION_UP	4
#define DIRECTION_DOWN	8

const int Menu::ITEMCOUNT = /*5*/ 4;
const char * Menu::item[ITEMCOUNT] = {/*"MISSIONS", */"MULTIPLAYER", "ONLINE", "OPTIONS", "QUIT"};

Menu::Menu() {
}

Menu::~Menu() {
}

void Menu::run() {
	SDL_Event event;

	init();

	for (int i = 0; i < 4; i++) {
		input[i] = Main::instance->input[i];
		input[i]->set_delay();
	}

	Main::audio->play_music(MUSIC_TITLE);

	frame = 0;

	started = false;
	input_master = Main::instance->input_master;

	Main::autoreset = true;

	while (Main::running) {
		while (SDL_PollEvent(&event)) {
			Main::instance->handle_event(&event);
			for (int i = 0; i < 4; i++) {
				input[i]->handle_event(&event);
			}
		}
		process_cursor();

		process_playeranimation();
		playeranimation->move();

		draw();

		Main::instance->flip();
		frame++;

		if (!Main::running && Main::is_reset) {
			Main::running = true;
			Main::is_reset = false;

			if (input_master != NULL) {
				input_master->reset();
				input_master->set_delay();
			}

			started = false;
		}
	}
	Main::audio->stop_music();

	cleanup();
}

void Menu::draw() {
	int i;
	SDL_Surface * text;
	SDL_Rect rect, rect_s;
	SDL_Surface * screen;

	screen = Main::instance->screen;

	SDL_BlitSurface(Main::graphics->bg_menu, NULL, screen, NULL);

	rect.x = (WINDOW_WIDTH - title->w) / 2;
	rect.y = 40;
	SDL_BlitSurface(title, NULL, screen, &rect);

	// Tile border
	rect_s.x = 0;
	rect_s.y = 0;
	rect_s.w = TILE_W;
	rect_s.h = TILE_H;

	rect.x = ((WINDOW_WIDTH - MENU_ITEM_WIDTH) / 2) - (TILE_W * 2);
	rect.y = MENU_TOP_OFFSET - TILE_H;
	for (i = 0; i < (MENU_ITEM_WIDTH / TILE_W) + 4; i++) {
		SDL_BlitSurface(Main::graphics->tiles, &rect_s, screen, &rect);
		rect.x += TILE_W;
	}

	for (i = 0; i < ITEMCOUNT; i++) {
		rect.x = surf_items_clip->at(i)->x - (TILE_W * 2);
		rect.y = surf_items_clip->at(i)->y - 8;
		SDL_BlitSurface(Main::graphics->tiles, &rect_s, screen, &rect);
		rect.x = surf_items_clip->at(i)->x + MENU_ITEM_WIDTH + TILE_W;
		SDL_BlitSurface(Main::graphics->tiles, &rect_s, screen, &rect);
	}

	rect.x = ((WINDOW_WIDTH - MENU_ITEM_WIDTH) / 2) - (TILE_W * 2);
	rect.y = MENU_TOP_OFFSET + (ITEMCOUNT * MENU_ITEM_HEIGHT);
	for (i = 0; i < (MENU_ITEM_WIDTH / TILE_W) + 4; i++) {
		SDL_BlitSurface(Main::graphics->tiles, &rect_s, screen, &rect);
		rect.x += TILE_W;
	}

	if (started) {
		// The menu
		for (i = 0; i < ITEMCOUNT; i++) {
			text = surf_items->at(i);

			if (selected_item == i) {
				rect.x = surf_items_clip->at(i)->x - TILE_W;
				rect.y = surf_items_clip->at(i)->y - 8;
				rect.w = MENU_ITEM_WIDTH + (TILE_W * 2);
				rect.h = MENU_ITEM_HEIGHT;

				SDL_FillRect(screen, &rect, 0x0088ff);
			}

			SDL_BlitSurface(text, NULL, screen, surf_items_clip->at(i));
		}
	} else {
		// Press start
		if (!(frame & 0x20) || !(frame & 0x8)) {
			rect.x = (WINDOW_WIDTH - Main::graphics->text_pressstart->w) / 2;
			rect.y = ((WINDOW_HEIGHT - Main::graphics->text_pressstart->h) / 2) - 16;
			SDL_BlitSurface(Main::graphics->text_pressstart, NULL, screen, &rect);
		}
	}

	// Player animation
	playeranimation->draw(screen);

	// Player name
	if (draw_playername) {
		SDL_BlitSurface(Main::graphics->playername->at(playeranimation->character), 0, screen, &pos_playername);
	}

	// Credits
	rect.x = (WINDOW_WIDTH - credits->at(0)->w) / 2;
	rect.y = WINDOW_HEIGHT - 35;
	SDL_BlitSurface(credits->at(0), NULL, screen, &rect);

	rect.x = (WINDOW_WIDTH - credits->at(1)->w) / 2;
	rect.y = WINDOW_HEIGHT - 25;
	SDL_BlitSurface(credits->at(1), NULL, screen, &rect);

	rect.x = (WINDOW_WIDTH - credits->at(2)->w) / 2;
	rect.y = WINDOW_HEIGHT - 15;
	SDL_BlitSurface(credits->at(2), NULL, screen, &rect);
}

void Menu::process_cursor() {
	if (started) {
		if (input_master->is_pressed(A_RUN) || input_master->is_pressed(A_JUMP) ||
			input_master->is_pressed(A_SHOOT) || input_master->is_pressed(A_BOMB)) {
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
				Main::audio->play(SND_SELECT);
				started = true;

				Main::instance->input_master = input[i];
				input_master = input[i];
			}
		}
	}
}

void Menu::select() {
	Main::audio->play(SND_SELECT);
	switch (selected_item + 1) {
		case 0:
			start_missions();
			break;
		case 1:
			start_local_multiplayer();
			break;
		case 2:
			start_server_listing();
			break;
		case 3:
			Options * options;
			options = new Options();
			options->run();
			delete options;
			break;
		case 4:
			SDL_Delay(500);
			Main::running = false;
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

	player = new Player(0, 1);
	player->input = input_master;
	player->bullets = 0;
	player->bombs = 0;

	ms = new MissionSelect();

	while (Main::running && running && !ms->cancel) {
		ms->run();

		if (Main::running && running && !ms->cancel) {
			level = new Level();
			level->load(Mission::MISSIONS[ms->mission].filename);

			m = new Mission();
			m->add_player(player);
			m->set_level(level);
			m->run();

			delete level;
			delete m;
		}
	}

	delete ms;
	delete player;

	Main::audio->play_music(MUSIC_TITLE);
}

void Menu::start_local_multiplayer() {
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
		player[i] = new Player(0, (i + 1));
		player[i]->input = input[i];
	}

	running = true;
	round = 0;

	cs = new CharacterSelect();
	ls = new LevelSelect();

	change_level = true;
	change_character = true;

	while (Main::running && running) {
		round++;

		if (change_character) {
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
			ls->run();
			if (ls->cancel) break;

			if (level != NULL)
				delete level;

			level = new Level();
			level->load(Level::LEVELS[ls->level].filename);
		}

		lmp = new LocalMultiplayer();
		lmp->set_level(level);

		for (int i = 0; i < 4; i++) {
			if (cs->player_joined[i])
				lmp->add_player(player[i]);
		}
		lmp->run();

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

		end = new LocalMultiplayerRoundEnd();
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

	Main::audio->play_music(MUSIC_TITLE);
}

void Menu::start_server_listing() 
{
	if (serverToken_.empty()) {
		rest::Token token;
		try {
			serverToken_ = token.get();
		} catch (std::runtime_error &exception) {
			show_error(exception.what());
		}
	}

	char secretKey[] = {
		0x56, 0xda, 0xce, 0x87, 0x52, 0x85, 0x50, 0xf1, 0xdd, 0x0c, 0x86, 0x92, 0x33, 0x49,
		0x21, 0xf4, 0x92, 0x23, 0x2b, 0xf3, 0x0c, 0x31, 0x23, 0x0e, 0xae, 0x49, 0x83, 0x92,
		0x2a, 0xdf, 0x9c, 0x8d
	};

	std::string secretKeyString(secretKey, sizeof (secretKey));
	std::string secretKeyStringHex(string_to_hex(secretKeyString));

	unsigned char md[32];
	std::string inpstr = serverToken_, sha256randomhash;

	inpstr.append(secretKeyStringHex);
	sha256((void *) inpstr.c_str(), inpstr.length(), md);

	char temp[4 + 1] = {0x00};
	for (int i = 0; i < 32; i++) {
		sprintf(temp, "%02x", md[i]);
		sha256randomhash.append(temp);
	}

	std::cout << "our generated token is: " << sha256randomhash << std::endl;

	rest::ServerList slist(sha256randomhash);
	try {
		json::Array servers = slist.list();

		ServerListing listOpts(servers);
		listOpts.run();


	} catch (std::runtime_error &exception) {
		show_error(exception.what());
	}
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
	if (playeranimation->character == 2 && frame - animation_start == 100) {
		playeranimation->is_duck = true;
	}
	if (frame - animation_start == 150) {
		playeranimation->is_duck = false;
		playeranimation->is_walking = true;
		playeranimation->direction = -1;
	}
	if (!draw_playername && playeranimation->momentumx == 0) {
		draw_playername = true;
		pos_playername.x = (playeranimation->position->x + (playeranimation->position->w / 2)) - (Main::graphics->playername->at(playeranimation->character)->w / 2);
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

	playeranimation = new PlayerAnimation((character + 1) % Player::CHARACTER_COUNT);
	animation_start = frame;
}

void Menu::select_up() {
	Main::audio->play(SND_SELECT);

	selected_item--;

	if (selected_item < 0) {
		selected_item = ITEMCOUNT - 1;
	}
}

void Menu::select_down() {
	Main::audio->play(SND_SELECT);

	selected_item++;

	if (selected_item == ITEMCOUNT) {
		selected_item = 0;
	}
}

void Menu::init() {
	SDL_Surface * surface;
	SDL_Rect * rect;

	selected_item = 0;

	title = Main::text->render_text_large("SMASH BATTLE");

	surf_items = new std::vector<SDL_Surface*>(0);
	surf_items_clip = new std::vector<SDL_Rect*>(0);
	for (int i = 0; i < ITEMCOUNT; i++) {
		surface = Main::text->render_text_medium(item[i]);
		surf_items->push_back(surface);

		rect = new SDL_Rect();
		rect->x = (WINDOW_WIDTH - MENU_ITEM_WIDTH) / 2;
		rect->y = MENU_TOP_OFFSET + (i * MENU_ITEM_HEIGHT) + 8;
		surf_items_clip->push_back(rect);
	}

	credits = new std::vector<SDL_Surface*>(0);
	surface = Main::text->render_text_small("PROGRAMMING BY BERT HEKMAN");
	credits->push_back(surface);
	surface = Main::text->render_text_small("GRAPHICS BY JEROEN GROENEWEG AND OKKE VOERMAN");
	credits->push_back(surface);
	surface = Main::text->render_text_small("MUSIC BY NICK PERRIN");
	credits->push_back(surface);

	playeranimation = new PlayerAnimation(0);
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

	for (unsigned int i = 0; i < credits->size(); i++) {
		SDL_FreeSurface(credits->at(i));
	}
	credits->clear();
	delete credits;

	delete playeranimation;
}

void Menu::show_error(const std::string &error_msg) {

	SDL_Surface *screen = Main::instance->screen;
	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
	SDL_Event event;

	while (true) {
		while (SDL_PollEvent(&event)) {
			Main::instance->handle_event(&event);
			for (int i = 0; i < 4; i++) {
				input[i]->handle_event(&event);
			}
		}

		if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
			break;

		short textpos = 10;
		SDL_Rect textLocation = {10, textpos, 0, 0};
		textpos += 24;
		std::string error_msg_up = error_msg;
		to_upper<char>(error_msg_up);
		SDL_Surface* textSurface = Main::text->render_text_small(error_msg_up.c_str());

		SDL_BlitSurface(textSurface, NULL, screen, &textLocation);
		SDL_FreeSurface(textSurface);

		Main::instance->flip(true);
	}
}