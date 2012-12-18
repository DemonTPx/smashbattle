#include "SDL/SDL.h"

#include "ClientNetworkMultiplayer.h"

#include "ServerClient.h"
#include "Server.h"
#include "Commands.hpp"
#include <map>
#include <algorithm>
#include <string>

using std::map;
using std::for_each;
using std::string;

#include "ClientNetworkMultiplayer.h"
#include "CharacterSelect.h"
#include "log.h"

void ClientNetworkMultiplayer::start()
{
	Level level;

	Main::instance->running = true;

	CharacterSelect cs(1, 1);
	cs.run();
	if (!cs.cancel) 
	{
		Player player(0, 0);				

		player.input = Main::instance->input[0];
		player.input->set_delay();
		player.set_character(cs.player_select[0]);

		add_player(&player);

		screen = Main::instance->screen;
		
		Uint32 begin = SDL_GetTicks();
		Uint32 calculatedLag = begin;
		int initialLagTests = INITIAL_LAG_TESTS;
		
		SDL_Event event;

		bool stop = false;
		while (!stop) {
			while(SDL_PollEvent(&event))
				;

			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
				break;
		
			if (!ServerClient::getInstance().isConnected()) {
				begin = SDL_GetTicks();
				static bool once = true;
				if (once) {
					once = false;

					try 
					{
						ServerClient::getInstance().connect(*this, level, player);
					}
					catch (std::runtime_error &error)
					{
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

			if (ServerClient::getInstance().getState() == ServerClient::State::INITIALIZED)
			{
				lag_ = &(ServerClient::getInstance().getLag());
				run();
				stop = true;
			}
		}
	}
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
		SDL_Rect textLocation = { 10, textpos, 0, 0 };
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
	short previous_test = flags;
		
	flags = 0;
	if (player.input->is_pressed(A_LEFT))		flags |= ServerClient::FLAG_LEFT;
	if (player.input->is_pressed(A_RIGHT))		flags |= ServerClient::FLAG_RIGHT;
	if (player.input->is_pressed(A_UP))			flags |= ServerClient::FLAG_UP;
	if (player.input->is_pressed(A_DOWN))		flags |= ServerClient::FLAG_DOWN;
	if (player.input->is_pressed(A_JUMP))		flags |= ServerClient::FLAG_JUMP;
	if (player.input->is_pressed(A_RUN))		flags |= ServerClient::FLAG_RUN;
	if (player.input->is_pressed(A_SHOOT))		flags |= ServerClient::FLAG_SHOOT;
	if (player.input->is_pressed(A_BOMB))		flags |= ServerClient::FLAG_BOMB;
	if (player.input->is_pressed(A_START))		flags |= ServerClient::FLAG_START;

	if (flags != previous_test)
	{
		CommandSetPlayerData req;
		req.data.client_id = ServerClient::getInstance().getClientId();
		req.data.time = SDL_GetTicks();
		req.data.flags = flags;
		req.data.x = player.position->x;
		req.data.y = player.position->y;


		req.data.momentumx = player.momentumx;
		req.data.momentumy = player.momentumy;
		req.data.newmomentumx = player.newmomentumx;
		req.data.is_running = player.is_running;
		req.data.is_duck = player.is_duck;
		req.data.is_duck_forced = player.is_duck_forced;
		req.data.duck_force_start = player.duck_force_start;
		req.data.is_jumping = player.is_jumping;
		req.data.is_falling = player.is_falling;
		req.data.jump_start = player.jump_start;
		//int current_sprite;
		//req.data.cycle_direction = player.cycle_direction;
		//req.data.distance_walked = player.distance_walked;

		ServerClient::getInstance().send(req);
	}
		
	for (map<int, Client>::iterator i =Server::getInstance().clients_.begin();
		i!=Server::getInstance().clients_.end();
		i++)
	{
		Client &client(i->second);
		short flags = client.test;
			
		Player &otherplayer(*(Gameplay::instance->players)->at(0));

		memset(otherplayer.input->pressed, 0x00, sizeof(otherplayer.input->pressed));
		if (flags & ServerClient::FLAG_LEFT)	otherplayer.input->pressed[A_LEFT] = true;
		if (flags & ServerClient::FLAG_RIGHT) 	otherplayer.input->pressed[A_RIGHT] = true;
		if (flags & ServerClient::FLAG_UP)		otherplayer.input->pressed[A_UP] = true;
		if (flags & ServerClient::FLAG_DOWN)	otherplayer.input->pressed[A_DOWN] = true;
		if (flags & ServerClient::FLAG_JUMP)	otherplayer.input->pressed[A_JUMP] = true;
		if (flags & ServerClient::FLAG_RUN)		otherplayer.input->pressed[A_RUN] = true;
		if (flags & ServerClient::FLAG_SHOOT)	otherplayer.input->pressed[A_SHOOT] = true;
		if (flags & ServerClient::FLAG_BOMB)	otherplayer.input->pressed[A_BOMB] = true;
		if (flags & ServerClient::FLAG_START)	otherplayer.input->pressed[A_START] = true;
	}
};