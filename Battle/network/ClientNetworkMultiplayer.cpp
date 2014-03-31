#include "SDL/SDL.h"

#include "network/ClientNetworkMultiplayer.h"

#include "network/ServerClient.h"
#include "network/Server.h"
#include "network/Commands.hpp"
#include "Main.h"
#include <map>
#include <algorithm>
#include <string>

using std::map;
using std::for_each;
using std::string;

#include "util/Log.h"

namespace network{

ClientNetworkMultiplayer::ClientNetworkMultiplayer(Main &main)
: lag_(NULL), LocalMultiplayer(main), main_(main), start_pressed_(false), start_pressed_start(0)
{
	main.setGameplay(this);
}

void ClientNetworkMultiplayer::start()
{
	Level level(main_);

	main_.running = true;

	Player player(0, 0, main_);

	player.input = main_.input[0];
	player.input->set_delay();
	player.set_character(main_.getServerClient().getCharacter());

	add_player(&player);

	screen = main_.screen;

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

		if (!main_.getServerClient().isConnected()) {
			begin = SDL_GetTicks();
			
			if (once) {
				once = false;

				try {
					main_.getServerClient().connect(*this, level, player, main_);
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
		else if (main_.getServerClient().isCommTokenAvailable()) {
			Uint32 current = SDL_GetTicks();

			if (initialLagTests > 0) {
				if (calculatedLag - current > 200) {
					calculatedLag += 200;
					main_.getServerClient().test();
					initialLagTests--;
				}
			}
			else {
				// Todo: refactor, merge with code in Poll()
				if (calculatedLag - current > 1000)
				{
					calculatedLag += 1000;
					main_.getServerClient().test();
				}
			}
		}

		draw_console();

		main_.getServerClient().poll();

		main_.flip(true);

		if (main_.getServerClient().getState() == ServerClient::State::INITIALIZED) {
			lag_ = &(main_.getServerClient().getLag());

			main_.getServerClient().resetTimer();

			run();
			stop = true;
		}
	}
	
	main_.getServerClient().setState(ServerClient::State::INITIALIZING);
	main_.getServerClient().disconnect();
	
	if (main_.input_master) {
		main_.input_master->set_delay(20);
	}
}

void ClientNetworkMultiplayer::draw_console()
{
	if (main_.no_sdl)
		return;

	screen = main_.screen;

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
		SDL_Surface* textSurface = main_.text->render_text_medium(text.c_str());
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
	if (main_.getServerClient().isConnected())
	{
		if (main_.getServerClient().getGame().is_ended() || main_.getServerClient().getGame().is_countdown())
		{
			flags = 0;
			return;
		}
	}

	short previous_flags = flags;
		
	CommandSetPlayerData req;
	player_util::set_position_data(req, main_.getServerClient().getClientId(), SDL_GetTicks(), main_.getServerClient().getUdpSeq(), player);
	flags = req.data.flags;

	if (flags != previous_flags)
		main_.getServerClient().send(req);
};

GameplayObject *ClientNetworkMultiplayer::generate_powerup(bool force)
{
	/* all handled by the server */
	return NULL;
}

void ClientNetworkMultiplayer::handle_pause_input(SDL_Event * event)
{
	if (start_pressed_ && frame - start_pressed_start >= 60) {
		start_pressed_ = false;
	}

	Player * player;
	for (unsigned int i = 0; i < players->size(); i++) {
		player = players->at(i);

		if (player->input->is_pressed(A_START)) {

			if (start_pressed_) {
				game_running = false;
			}
			else {
				start_pressed_ = true;
				start_pressed_start = frame;
				broadcast_duration = frame + 1000;
				broadcast_msg = "PRESS START AGAIN TO LEAVE THIS GAME";
			}
		}
	}
}

}
