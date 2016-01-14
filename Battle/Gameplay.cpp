#include "SDL/SDL.h"

#include <vector>
#include <sstream>

#include "Main.h"
#include "Level.h"
#include "Player.h"
#include "GameplayObject.h"
#include "PauseMenu.h"

#include "Gameplay.h"

#include "network/ServerClient.h"
#include "commands/CommandApplyPowerup.hpp"

#ifndef WIN32
#define sprintf_s snprintf
#endif

Gameplay::Gameplay(Main &main) : main_(main) {

	frame = 0;

	ticks_start = SDL_GetTicks();
	
	game_running = false;
	server_game_running = false;
	players = new std::vector<Player*>();
	npcs = new std::vector<NPC*>();
	objects = new std::vector<GameplayObject*>();
	
	players_collide = true;
	npcs_collide = true;
	players_npcs_collide = true;

	broadcast_duration = 0;

	disconnected_time_ = 0;
}

Gameplay::~Gameplay() {
}

#include "network/Server.h"
#include <map>
using std::map;
#include "network/Client.h"
#include "network/Server.h"

void Gameplay::run() {
	SDL_Event event;

	screen = main_.screen;

	main_.autoreset = false;

	initialize();

	reset_game();

	game_running = true;
	main_.audio->stop_music();
	music_playing = false;
	
	// Set the input defaults
	for(unsigned int idx = 0; idx < players->size(); idx++) {
		Player * p = players->at(idx);
		p->input->unset_delay();
	}

	// Get ticks (milliseconds since SDL started)
	ticks_start = SDL_GetTicks();

	while(main_.running && (game_running || server_game_running)) {
		if (!main_.no_sdl) {
			// Event handling
			while(SDL_PollEvent(&event)) {
				main_.handle_event(&event);

				// Handle player input
				for(unsigned int idx = 0; idx < players->size(); idx++) {
					Player * p = players->at(idx);
					p->input->handle_event(&event);
				}

				handle_pause_input(&event);
			}
		}

		on_input_handled();

		int frames_processed = 0;

		// Calculate difference in milliseconds since our previous measure
		Sint32 ticks_diff = SDL_GetTicks() - ticks_start;

		// If enough time has passed skip frame(s)
		while (ticks_diff >= main_.MILLISECS_PER_FRAME)
		{
			// Update our 'previous'/start measure and diff
			ticks_start += main_.MILLISECS_PER_FRAME;
			ticks_diff -= main_.MILLISECS_PER_FRAME;

			// Gameplay processing
			on_pre_processing();
			if(!countdown && !ended) {
				// Move and process players
				for(unsigned int idx = 0; idx < players->size(); idx++) {
					Player * p = players->at(idx);
					p->move(level);
					switch (main_.runmode)
					{
						case MainRunModes::SERVER:
							// Server gets projectiles from client through commands
							break;
						case MainRunModes::CLIENT:
							// Client will only process and send it's own projectiles to server, 
							//  for other player bullets we depend on the server for sending them.
							if (p->number == main_.getServerClient().getClientId())
								p->process();
							break;
						default:
							p->process();
							break;
					}
				}

				process_player_collission();

				// Move and process NPCs
				for(unsigned int idx = 0; idx < npcs->size(); idx++) {
					NPC * npc = npcs->at(idx);
					npc->move(level);
					npc->process();

					if(npc->done) {
						npcs->erase(npcs->begin() + idx);
						delete npc;
					}
				}

				process_npc_collission();

				process_player_npc_collission();

				// Move and process objects
				process_gameplayobj();

			}
			if(countdown) {
				process_countdown();
			}

			on_post_processing();

			// Next frame
			frame++;
			frames_processed++;
		}

		// Drawing
		if (!main_.no_sdl) {
			level->draw(screen, frames_processed);

			for(unsigned int idx = 0; idx < players->size(); idx++) {
				Player * p = players->at(idx);
				if(countdown)
					p->draw(screen, true, frames_processed);
				else
					p->draw(screen, false, frames_processed);
			}
			for(unsigned int idx = 0; idx < npcs->size(); idx++) {
				NPC * npc = npcs->at(idx);
				npc->draw(screen, frames_processed);
			}

			for(unsigned int idx = 0; idx < objects->size(); idx++) {
				GameplayObject * obj = objects->at(idx);
				obj->draw(screen, frames_processed);
			}

			draw_score();
		}
		
		if(ended) {
			if (!main_.no_sdl) {
				draw_game_ended();
			}

			if (main_.runmode == MainRunModes::CLIENT) {
				// reset games are handled by server
			}
			else if(frame - end_start > 120) {
				reset_game();
			}
		}
		if(countdown) {
			if (!main_.no_sdl) {
				draw_countdown();
			}
		}

		if (main_.runmode == MainRunModes::CLIENT)
		{
			if (!main_.getServerClient().isConnected()) {
				draw_disconnected();
				if (disconnected_time_ == 0) {
					disconnected_time_ = main_.getServer().getServerTime();
				}
				else {
					if ((main_.getServer().getServerTime() - disconnected_time_) >= 3000) {
						disconnected_time_ = 0;
						break;
					}
				}
			}
			else if (main_.getServerClient().showConsole())
				draw_console();
		}

		if (!main_.no_sdl) {
			draw_broadcast();
		}
		if (broadcast_duration > 0) {
			for (int i=0; i<frames_processed; i++) {
				broadcast_duration -= main_.MILLISECS_PER_FRAME;
				if (broadcast_duration < 0)
					broadcast_duration = 0;
			}
		}

		main_.flip(true);
	}

	deinitialize();

	main_.autoreset = true;
}

void Gameplay::move_player(Player &player)
{
	// Move (do not process)
	player.move(level);

	process_player_collission();

	process_npc_collission();

	process_player_npc_collission();

	// Process collisions with gameplay objects
	for(unsigned int idx = 0; idx < objects->size(); idx++) {
		GameplayObject * obj = objects->at(idx);
		for(unsigned int i = 0; i < players->size(); i++) {
			Player * p = players->at(i);
			if(p->is_dead)
				continue;
			std::unique_ptr<SDL_Rect> rect(p->get_rect());
			if(is_intersecting(rect.get(), obj->position)) {
 				// In case the runmode is client, and it is a powerup, skip hit_player() call, we receive these commands from the server
				if (!obj->is_powerup || main_.runmode != MainRunModes::CLIENT) {
					// In case the runmode is server, and it's a powerup, send a hit notification to clients, so they can process the powerup
					if (obj->is_powerup && main_.runmode == MainRunModes::SERVER) {
						network::CommandApplyPowerup apply;
						apply.data.time = main_.getServer().getServerTime();
						apply.data.player_id = p->number;
						apply.data.powerup_id = obj->id();
						main_.getServer().sendAll(apply);
					}
					obj->hit_player(p);
				}
			}
		}
		for(unsigned int i = 0; i < npcs->size(); i++) {
			NPC * npc = npcs->at(i);
			if(npc->is_dead)
				continue;
			std::unique_ptr<SDL_Rect> rect(npc->get_rect());
			if(is_intersecting(rect.get(), obj->position)) {
				obj->hit_npc(npc);
			}
		}
		if(obj->done) {
			objects->erase(objects->begin() + idx);
			delete obj;
		}
	}

}

/**
 * If projectile is NULL, process all otherwise only given gameplayobject
 returns false if the object is deleted
 */
bool Gameplay::process_gameplayobj(GameplayObject *gameplayobj)
{
	// Move and process objects
	for(unsigned int idx = 0; idx < objects->size(); idx++) {
		GameplayObject * obj = objects->at(idx);

		if (gameplayobj != NULL && gameplayobj != obj)
			continue;
		
		obj->move(level);
		obj->process();
		for(unsigned int i = 0; i < players->size(); i++) {
			Player * p = players->at(i);
			if(p->is_dead)
				continue;
			SDL_Rect * rect;
			rect = p->get_rect();
			if(is_intersecting(rect, obj->position)) {
 				// In case the runmode is client, and it is a powerup, skip hit_player() call, we receive these commands from the server
				if (!obj->is_powerup || main_.runmode != MainRunModes::CLIENT)
				{
					// In case the runmode is server, and it's a powerup, send a hit notification to clients, so they can process the powerup
					if (obj->is_powerup && main_.runmode == MainRunModes::SERVER) {
						network::CommandApplyPowerup apply;
						apply.data.time = main_.getServer().getServerTime();
						apply.data.player_id = p->number;
						apply.data.powerup_id = obj->id();
						main_.getServer().sendAll(apply);
					}
					obj->hit_player(p);
				}
			}
			delete rect;
		}
		for(unsigned int i = 0; i < npcs->size(); i++) {
			NPC * npc = npcs->at(i);
			if(npc->is_dead)
				continue;
			SDL_Rect * rect;
			rect = npc->get_rect();
			if(is_intersecting(rect, obj->position)) {
				obj->hit_npc(npc);
			}
			delete rect;
		}
		if(obj->done) {
			objects->erase(objects->begin() + idx);
			delete obj;
			return false;
		}
	}
	return true;
}

void Gameplay::handle_pause_input(SDL_Event * event) {
	Player * player;
	for(unsigned int i = 0; i < players->size(); i++) {
		player = players->at(i);

		if(player->input->is_pressed(A_START)) {
			pause(player);
		}
	}
}

void Gameplay::pause(Player * p) {
	int ret;
	ret = pause_menu->pause(p);
	
	if(ret == 1) game_running = false;
	
	// In Arcade mode time does not elapse in pause menu
	if (main_.runmode == MainRunModes::ARCADE) {
		ticks_start = SDL_GetTicks();
	}
}

void Gameplay::set_level(Level * l) {
	if(!game_running) {
		level = l;
	}
}

void Gameplay::add_player(Player * p) {
	players->push_back(p);
}

void Gameplay::del_player_by_id(char number)
{
	std::unique_ptr<Player> deletePlayer;
	for (std::vector<Player *>::iterator i = players->begin(); i!= players->end(); i++)
	{
		Player *player(*i);

		if (number == player->number)
		{
			deletePlayer = std::move(std::unique_ptr<Player>(player));
			on_pre_delete_player(*deletePlayer.get());
			players->erase(i);
			return;
		}
	}
}

void Gameplay::del_players()
{
	players->clear();
}

void Gameplay::del_other_players()
{
	while (players->size() > 1)
		players->erase(players->begin()++);
}

void Gameplay::add_npc(NPC * npc) {
	npcs->push_back(npc);
}

void Gameplay::bounce_up_players_and_npcs(SDL_Rect * rect, SDL_Rect * source, Player * initialized_by_player) {
	Player * p;
	for(unsigned int i = 0; i < players->size(); i++) {
		p = players->at(i);
		if(is_intersecting(rect, p->position)) {
			p->last_pushed_player = initialized_by_player;
			p->last_damage_move = KillMove::FALLING;
			p->bounce_up(source);
		}
	}

	NPC * npc;
	for(unsigned int i = 0; i < npcs->size(); i++) {
		npc = npcs->at(i);
		if(is_intersecting(rect, npc->position))
			npc->bounce_up(source);
	}
}

void Gameplay::add_object(GameplayObject * obj) {
	objects->push_back(obj);
}

void Gameplay::reset_game() {
	frame = 0;
	
	//paused = false;
	ended = false;
	countdown = true;
	
	countdown_sec_left = 5;
	countdown_start = frame;
	strcpy(countdown_pre_text, "GET READY");

	// In network::ServerClient upon receiving the level from server, we reset already, and then initialize the tiles
	if (main_.runmode != MainRunModes::CLIENT)
	{
		level->reset();
	}
	
	// Clear gameplay objects
	GameplayObject * obj;
	for(unsigned int i = 0; i < objects->size(); i++) {
		obj = objects->at(i);
		delete obj;
	}
	objects->clear();

	on_game_reset();

	//main_.audio->stop_music();

	srand(SDL_GetTicks());
}

void Gameplay::initialize() {
	pause_menu = new PauseMenu(screen, main_);
	pause_menu->add_option((char*)"RESUME\0");
	pause_menu->add_option((char*)"QUIT\0");
}

void Gameplay::deinitialize() {
	// Stop the music
	if(!countdown)
		main_.audio->stop_music();

	// Clear players (we don't delete the players, because they are not created in this class)
	players->clear();
	delete players;

	// Clear NPC's
	NPC * npc;
	for(unsigned int i = 0; i < npcs->size(); i++) {
		npc = npcs->at(i);
		delete npc;
	}
	npcs->clear();
	delete npcs;

	// Clear gameplay objects
	GameplayObject * obj;
	for(unsigned int i = 0; i < objects->size(); i++) {
		obj = objects->at(i);
		delete obj;
	}
	objects->clear();
	delete objects;

	// Unload pause menu
	delete pause_menu;
}

void Gameplay::draw_countdown() {
	char text[5];
	SDL_Surface * surf;

	if(countdown_sec_left > 3) {
		surf = main_.text->render_text_medium_shadow(countdown_pre_text);
	} else {
		sprintf_s(text, 5, "%d", countdown_sec_left);
		surf = main_.text->render_text_large_shadow(text);
	}

	SDL_Rect rect;
	rect.x = (screen->w - surf->w) / 2;
	rect.y = (screen->h - surf->h) / 2;
	
	SDL_BlitSurface(surf, NULL, screen, &rect);
	SDL_FreeSurface(surf);
}

void Gameplay::draw_disconnected() 
{
	SDL_Surface * surf;

	surf = main_.text->render_text_medium("DISCONNECTED FROM SERVER");

	SDL_Rect rect;
	rect.x = (screen->w - surf->w) / 2;
	rect.y = (screen->h - surf->h) / 2;
	
	SDL_BlitSurface(surf, NULL, screen, &rect);
	SDL_FreeSurface(surf);
}

void Gameplay::draw_broadcast()
{
	if (broadcast_duration == 0)
		return;


	std::istringstream stream(broadcast_msg);
	std::string line;
	int offsetY = 0;
	while(std::getline(stream, line)) {
		SDL_Surface * surf = main_.text->render_text_medium_shadow(line.c_str());

		SDL_Rect rect;
		rect.x = (screen->w - surf->w) / 2;
		rect.y = ((screen->h - surf->h) / 2) - TILE_H + offsetY;
		
		SDL_BlitSurface(surf, NULL, screen, &rect);
		SDL_FreeSurface(surf);

		offsetY += 24;
	}
}


void Gameplay::draw_pause_screen() {

}

void Gameplay::draw_score() {

}

void Gameplay::draw_game_ended() {

}

bool Gameplay::is_intersecting(SDL_Rect * one, SDL_Rect * two) {
	bool intersect;
	int l1, r1, t1, b1;
	int l2, r2, t2, b2;

 	l1 = one->x;
	r1 = one->x + one->w;
	t1 = one->y;
	b1 = one->y + one->h;

	l2 = two->x;
	r2 = two->x + two->w;
	t2 = two->y;
	b2 = two->y + two->h;

	// Normal collision
	intersect = true;
	if(l1 >= r2) intersect = false;
	if(r1 <= l2) intersect = false;
	if(t1 >= b2) intersect = false;
	if(b1 <= t2) intersect = false;
	if(intersect) return true;

	// Collisions that go through the sides
	if(r1 >= WINDOW_WIDTH && r2 < WINDOW_WIDTH) {
		l1 -= WINDOW_WIDTH;
		r1 -= WINDOW_WIDTH;
		intersect = true;
		if(l1 > r2) intersect = false;
		if(r1 < l2) intersect = false;
		if(t1 > b2) intersect = false;
		if(b1 < t2) intersect = false;
	}
	else if(r2 >= WINDOW_WIDTH && r1 < WINDOW_WIDTH) {
		l2 -= WINDOW_WIDTH;
		r2 -= WINDOW_WIDTH;
		intersect = true;
		if(l1 > r2) intersect = false;
		if(r1 < l2) intersect = false;
		if(t1 > b2) intersect = false;
		if(b1 < t2) intersect = false;
	}

	return intersect;
}


#include "commands/CommandSetPlayerData.hpp"
void Gameplay::process_player_collission() {
	if(!players_collide)
		return;

	Player * p1, * p2;

	for(unsigned int i1 = 0; i1 < players->size(); i1++) {
		p1 = players->at(i1);
		
		if(p1->is_dead) continue;

		for(unsigned int i2 = (i1 + 1); i2 < players->size(); i2++) {
			p2 = players->at(i2);

			if(p2->is_dead) continue;

			std::unique_ptr<SDL_Rect> r1 (p1->get_rect());
			std::unique_ptr<SDL_Rect> r2 (p2->get_rect());

			if(is_intersecting(r1.get(), r2.get())) {
				main_.audio->play(SND_BOUNCE, (p1->position->x + p2->position->x) / 2);

				p1->bounce(p2);
				p2->bounce(p1);

				p1->momentumx = p1->newmomentumx;
				p2->momentumx = p2->newmomentumx;

				// We re-send our position to the server if we bounced someone, just in case some bounce didn't happen on the server.
				//  We cannot however send this update immediately, because if we send the new position *we* know of now, we may not bounce
				//  on the server (I learned this by actually implementing it wrong).
				// Therefore (re)set a timer that will periodically send our data to server.
				// But we cannot do this 
				if (main_.runmode == MainRunModes::CLIENT && main_.getServerClient().isConnected()) {
					if (main_.getServerClient().getClientId() == p1->number || main_.getServerClient().getClientId() == p2->number) {
						main_.getServerClient().resetTimer();
					}
				}
			}
		}
	}
}

void Gameplay::process_npc_collission() {
	if(!npcs_collide)
		return;

	NPC * c1, * c2;
	SDL_Rect * r1, * r2;

	for(unsigned int i1 = 0; i1 < npcs->size(); i1++) {
		c1 = npcs->at(i1);
		
		if(c1->is_dead) continue;

		for(unsigned int i2 = (i1 + 1); i2 < npcs->size(); i2++) {
			c2 = npcs->at(i2);

			if(c2->is_dead) continue;

			r1 = c1->get_rect();
			r2 = c2->get_rect();

			if(is_intersecting(r1, r2)) {
				c1->bounce(c2);
				c2->bounce(c1);

				c1->momentumx = c1->newmomentumx;
				c2->momentumx = c2->newmomentumx;
			}

			delete r1;
			delete r2;
		}
	}
}


void Gameplay::process_player_npc_collission() {
	Player * player;
	NPC * npc;
	SDL_Rect * r_player, * r_npc;

	for(unsigned int i1 = 0; i1 < players->size(); i1++) {
		player = players->at(i1);
		
		if(player->is_dead) continue;

		for(unsigned int i2 = 0; i2 < npcs->size(); i2++) {
			npc = npcs->at(i2);

			if(npc->is_dead) continue;

			r_player = player->get_rect();
			r_npc = npc->get_rect();

			if(is_intersecting(r_player, r_npc)) {
				npc->bounce(player);

				player->momentumx = player->newmomentumx;
				npc->momentumx = npc->newmomentumx;
			}

			delete r_player;
			delete r_npc;
		}
	}
}

void Gameplay::process_countdown() {
	if(frame - countdown_start >= 60) {
		if(countdown_sec_left == 1) {
			countdown = false;
			main_.audio->play(SND_GO);
			if(!music_playing) {
				main_.audio->play_music(level->music);
				music_playing = true;
			}
			return;
		}
		countdown_sec_left--;
		countdown_start = frame;

		if(countdown_sec_left <= 3)
			main_.audio->play(SND_COUNTDOWN);
	}
}

void Gameplay::set_broadcast(std::string msg, int duration) 
{
	broadcast_msg = msg;
	broadcast_duration = duration;
}