#include "SDL/SDL.h"
#include "ClientSettings.h"
#include "Options.h"
#include "PlayerAnimation.h"

ClientSettings::ClientSettings(Main &main)
: OptionsScreen("SET UP YOUR CHARACTER", main), main_(main)
{
	character_ = (int) main.online_character;
	anim = new PlayerAnimation(character_, main);
	selectServerText_ = "SELECT SERVER";
	initialize();
}

ClientSettings::~ClientSettings()
{
}

/**
 * There is a memory leak in this, which is more easily fixed if OptionsScreen
 * would be refactored, but this is a nice to have, I'll leave it like this for now..
 */
void ClientSettings::initialize()
{
	create_items();

	OptionsScreen::align = LEFT;

	init();
}

#include "CharacterSelect.h"
#include "ServerListing.h"
#include "network/ServerClient.h"

void ClientSettings::item_selected()
{
	std::cout << " Selected item is: " << selected_item << std::endl;

	switch (selected_item) {
		case 0:
		{
			CharacterSelect cs(1, 1, main_);
			cs.player_select_first = character_;
			cs.run();

			if (!cs.cancel) {
				anim->set_character(cs.player_select[0]);
				std::cout << anim->character << std::endl;

				character_ = cs.player_select[0];
				main_.online_character = (Uint8)character_;
			}
			break;
		}
		case 1:
		{
			main_.getServerClient().setCharacter(character_);
			connect();
			break;
		}
	}
	create_items();
	update();
}

void ClientSettings::on_pre_draw()
{
}

void ClientSettings::on_post_draw()
{
	anim->move();
	anim->animate_in_place = true;
	anim->is_walking = true;
	anim->position->x = 400;
	anim->position->y = 68;
	anim->direction = -1;
	SDL_Surface *screen = main_.screen;

	char *name = Player::CHARACTERS[1].name;

	anim->draw(screen);
}

void ClientSettings::create_items()
{
	items->clear();

	OptionItem * item;

	text = "CHARACTER: ";
	text.append(Player::CHARACTERS[character_].name);

	item = new OptionItem();
	item->name = const_cast<char *> (text.c_str());
	item->options = NULL;
	item->selected = 0;
	add_item(item);

	item = new OptionItem();
	item->name = const_cast<char *>(selectServerText_.c_str());
	item->options = NULL;
	item->selected = 0;
	add_item(item);
}

#include <algorithm>
#include <stdexcept>
#include "ServerListing.h"
#include "rest/ClientToken.h"
#include "rest/ServerList.h"
#include "util/json.h"
#include "util/stringutils.hpp"
#include "util/sha256.h"

void ClientSettings::connect()
{
	if (serverToken_.empty()) {
		rest::ClientToken token;
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

		ServerListing listOpts(servers, main_);
		listOpts.run();


	} catch (std::runtime_error &exception) {
		show_error(exception.what());
	}
}

void ClientSettings::show_error(const std::string &error_msg)
{

	SDL_Surface *screen = main_.screen;
	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
	SDL_Event event;

	while (true) {
		while (SDL_PollEvent(&event)) {
			main_.handle_event(&event);
			for (int i = 0; i < 4; i++) {
				main_.input[i]->handle_event(&event);
			}
		}

		if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
			break;

		short textpos = 10;
		SDL_Rect textLocation = {10, textpos, 0, 0};
		textpos += 24;
		std::string error_msg_up = error_msg;
		to_upper<char>(error_msg_up);
		SDL_Surface* textSurface = main_.text->render_text_small(error_msg_up.c_str());

		SDL_BlitSurface(textSurface, NULL, screen, &textLocation);
		SDL_FreeSurface(textSurface);

		main_.flip(true);
	}
}
