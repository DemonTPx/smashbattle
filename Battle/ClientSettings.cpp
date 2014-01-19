#include "SDL/SDL.h"
#include "ClientSettings.h"
#include "Options.h"
#include "PlayerAnimation.h"

ClientSettings::ClientSettings()
: OptionsScreen("SET UP YOUR CHARACTER"), nickname_("TWEAKER"), character_(0), anim(new PlayerAnimation(1)),
oitem1_(NULL), oitem2_(NULL), oitem3_(NULL), editting_nickname_(false)
{
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
	items->clear();

	item1_ = "NICKNAME: ";
	item1_.append(nickname_);

	std::cout << item1_ << std::endl;

	oitem1_ = new OptionItem();
	oitem1_->name = const_cast<char *> (item1_.c_str());
	oitem1_->options = NULL;
	oitem1_->selected = 0;
	add_item(oitem1_);

	item2_ = "CHARACTER: ";
	item2_.append(Player::CHARACTERS[character_].name);

	oitem2_ = new OptionItem();
	oitem2_->name = const_cast<char *> (item2_.c_str());
	oitem2_->options = NULL;
	oitem2_->selected = 0;
	add_item(oitem2_);

	item3_ = "CONNECT";
	oitem3_ = new OptionItem();
	oitem3_->name = const_cast<char *> (item3_.c_str());
	oitem3_->options = NULL;
	oitem3_->selected = 0;
	add_item(oitem3_);

	OptionsScreen::align = LEFT;

	init();
}

void ClientSettings::run()
{
	OptionsScreen::run();

	std::cout << " after run" << std::endl;
}

bool ClientSettings::process_event(SDL_Event &event)
{
	if (!editting_nickname_) {
		return true;
	}

	if (event.type != SDL_KEYDOWN)
		return false;

	std::cout << "Do something with the event key!" << std::endl;

	std::cout << event.key.keysym.sym << std::endl;

	if (event.key.keysym.sym == 8) {
		nickname_ = nickname_.substr(0, nickname_.length() - 1);
	}
	if (event.key.keysym.sym >= 'a' && event.key.keysym.sym <= 'z') {
		nickname_ += (char) event.key.keysym.sym + ('A' - 'a');
	}
	if (event.key.keysym.sym >= '0' && event.key.keysym.sym <= '9') {
		nickname_ += (char) event.key.keysym.sym;
	}
	if (event.key.keysym.sym == '_' || event.key.keysym.sym == '-') {
		nickname_ += (char) event.key.keysym.sym;
	}

	if (event.key.keysym.sym == 27 || event.key.keysym.sym == 13) {
		std::cout << " Resetting this shit" << std::endl;

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			//Main::instance->handle_event(&event);

		}
		for (int i = 0; i < 4; i++) {
			//Main::instance->input[i]->set_delay();
			//Main::instance->input[i]->handle_event(&event);
			Main::instance->input[i]->reset();
		}

		Main::instance->input_master->reset();
		editting_nickname_ = false;
	}

	std::cout << "Nickname is nu: " << nickname_ << std::endl;
	return false;
}

void ClientSettings::process_cursor()
{
	if (!editting_nickname_) {
		OptionsScreen::process_cursor();
	}

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
			editting_nickname_ = true;
			break;
		}
		case 1:
		{
			CharacterSelect cs(1, 1);
			cs.run();

			if (!cs.cancel) {
				anim->set_character(cs.player_select[0]);
				std::cout << anim->character << std::endl;

				character_ = cs.player_select[0];
			}
			break;
		}
		case 2:
		{
			ServerClient::getInstance().setNickname(nickname_);
			ServerClient::getInstance().setCharacter(character_);
			connect();
			break;
		}
	}
	initialize();
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
	SDL_Surface *screen = Main::instance->screen;

	char *name = Player::CHARACTERS[1].name;


	anim->draw(screen);


	if (editting_nickname_) {
		SDL_Rect textLocation = {15, 150, 0, 0};
		std::string error_msg_up("CHOOSE NEW NICKNAME (ONLY A-Z 0-9 AND - ARE ALLOWED)");
		SDL_Surface* textSurface = Main::text->render_text_medium(error_msg_up.c_str());

		SDL_BlitSurface(textSurface, NULL, screen, &textLocation);
		SDL_FreeSurface(textSurface);

		textLocation.y = 200;
		textSurface = Main::text->render_text_medium(nickname_.c_str());
		SDL_BlitSurface(textSurface, NULL, screen, &textLocation);
		SDL_FreeSurface(textSurface);

		initialize();
	}
}

#include <algorithm>
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

		ServerListing listOpts(servers);
		listOpts.run();


	} catch (std::runtime_error &exception) {
		show_error(exception.what());
	}
}

void ClientSettings::show_error(const std::string &error_msg)
{

	SDL_Surface *screen = Main::instance->screen;
	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
	SDL_Event event;

	while (true) {
		while (SDL_PollEvent(&event)) {
			Main::instance->handle_event(&event);
			for (int i = 0; i < 4; i++) {
				Main::instance->input[i]->handle_event(&event);
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