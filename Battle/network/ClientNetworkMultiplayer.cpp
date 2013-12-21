#include "SDL/SDL.h"

#include "network/ClientNetworkMultiplayer.h"

#include "network/ServerClient.h"
#include "network/Server.h"
#include "network/Commands.hpp"
#include <map>
#include <algorithm>
#include <string>

using std::map;
using std::for_each;
using std::string;

#include "network/ClientNetworkMultiplayer.h"
#include "util/Log.h"

void ClientNetworkMultiplayer::start()
{
	Level level;

	Main::instance->running = true;

	Player player(0, 0);				

	player.input = Main::instance->input[0];
	player.input->set_delay();
	player.set_character(ServerClient::getInstance().getCharacter());

	add_player(&player);

	screen = Main::instance->screen;

	Uint32 begin = SDL_GetTicks();
	Uint32 calculatedLag = begin;
	int initialLagTests = INITIAL_LAG_TESTS;

	SDL_Event event;

	bool stop = false, once = true;;
	while (!stop) {
		while(SDL_PollEvent(&event))
			;

		if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
			break;

		if (!ServerClient::getInstance().isConnected()) {
			begin = SDL_GetTicks();
			
			if (once) {
				once = false;

				try {
					ServerClient::getInstance().connect(*this, level, player);
				}
				catch (std::runtime_error &error) {
					log(error.what(), Logger::Priority::CONSOLE);
				}
			}
			else
			{
				static bool once = true;
				if (once)
					log("DISCONNECTED FROM SERVER", Logger::Priority::CONSOLE);
				once = false;
			}

		}
		else {
			Uint32 current = SDL_GetTicks();

			if (initialLagTests > 0) {
				if (calculatedLag - current > 200) {
					calculatedLag += 200;
					ServerClient::getInstance().test();
					initialLagTests--;
				}
			}
			else {
				// Todo: refactor, merge with code in Poll()
				if (calculatedLag - current > 1000)
				{
					calculatedLag += 1000;
					ServerClient::getInstance().test();
				}
			}
		}

		draw_console();

		ServerClient::getInstance().poll();

		Main::instance->flip(true);

		if (ServerClient::getInstance().getState() == ServerClient::State::INITIALIZED) {
			lag_ = &(ServerClient::getInstance().getLag());

			ServerClient::getInstance().resetTimer();

			run();
			stop = true;
		}
	}
	
	std::cout << " Disconnecting etc. " << std::endl;
	
	ServerClient::getInstance().setState(ServerClient::State::INITIALIZING);
	ServerClient::getInstance().disconnect();
	
	Main::instance->input_master->set_delay(20);
}

void ClientNetworkMultiplayer::draw_console()
{
	screen = Main::instance->screen;

	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

	// Output console (exactly 20 lines fit)
	size_t beginSize = Logger::console.size();
	if (beginSize > 20)
		beginSize = beginSize - 20;
	else
		beginSize = 0;

	int textpos = 5;
	for_each(std::begin(Logger::console) + beginSize, std::end(Logger::console), [&](const string &str) {
		SDL_Rect textLocation = { 10, static_cast<short>(textpos), 0, 0 };
		textpos += 24;
		string text = str;
		to_upper<char>(text);
		SDL_Surface* textSurface = Main::text->render_text_medium(text.c_str());
		SDL_BlitSurface(textSurface, NULL, screen, &textLocation);
		SDL_FreeSurface(textSurface);
	});
}

void ClientNetworkMultiplayer::on_game_reset()
{
	countdown = false;
}

void ClientNetworkMultiplayer::on_input_handled() 
{
	Player &player(*players->at(0));
	static short flags = 0;
	if (player.is_dead) {
		flags = 0;
		return;
	}
	if (ServerClient::getInstance().isConnected())
	{
		if (ServerClient::getInstance().getGame().is_ended() || ServerClient::getInstance().getGame().is_countdown())
		{
			flags = 0;
			return;
		}
	}
	short previous_flags = flags;
		
	CommandSetPlayerData req;
	player_util::set_position_data(req, ServerClient::getInstance().getClientId(), SDL_GetTicks(), player);

	flags = req.data.flags;

	if (flags != previous_flags)
		ServerClient::getInstance().send(req);
};

GameplayObject *ClientNetworkMultiplayer::generate_powerup(bool force)
{
	/* all handled by the server */
	return NULL;
}