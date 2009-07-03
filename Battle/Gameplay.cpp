#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"

#include <vector>

#include "Main.h"
#include "Level.h"
#include "Player.h"
#include "GameplayObject.h"

#include "Gameplay.h"

int Gameplay::frame = 0;

Gameplay::Gameplay() {
	game_running = false;
	players = new std::vector<Player*>();
	objects = new std::vector<GameplayObject*>();
}

Gameplay::~Gameplay() {
	delete players;
	delete objects;
}

void Gameplay::run() {
	SDL_Event event;

	screen = Main::instance->screen;

	initialize();

	reset_game();

	game_running = true;

	while(Main::running && game_running) {
		// Event handling
		while(SDL_PollEvent(&event)) {
			Main::instance->handle_event(&event);

			/*
			handle_pause_input(&event);

			// Handle player input
			for(unsigned int idx = 0; idx < players->size(); idx++) {
				Player * p = players->at(idx);
				p->handle_event(&event);
			}
			*/
		}

		// Gameplay processing
		if(!paused && !countdown && !ended) {
			// Move and process players
			for(unsigned int idx = 0; idx < players->size(); idx++) {
				Player * p = players->at(idx);
				//p->move();
				//p->process();
			}

			process_player_collission();

			// Move and process NPCs
		/*	for(unsigned int idx = 0; idx < npcs->size(); idx++) {
				NPC * npc = npcs->at(idx);
				npc->move();
				npc->process();
			}*/

			process_npc_collission();

			// Move and process objects
			for(unsigned int idx = 0; idx < objects->size(); idx++) {
				GameplayObject * obj = objects->at(idx);
				obj->move();
				obj->process();
				if(obj->done) {
					objects->erase(objects->begin() + idx);
					delete obj;
				}
			}
		}

		// Drawing
		frame++;

		level->draw(screen);

		for(unsigned int idx = 0; idx < players->size(); idx++) {
			Player * p = players->at(idx);
			//p->draw(screen);
			p->show(screen);
		}
	/*	for(unsigned int idx = 0; idx < npcs->size(); idx++) {
			NPC * npc = npcs->at(idx);
			npc->draw(screen);
		}*/

		for(unsigned int idx = 0; idx < objects->size(); idx++) {
			GameplayObject * obj = objects->at(idx);
			obj->draw(screen);
		}

		// TODO: ending
		// TODO: countdown
		// TODO: pause screen

		Main::instance->flip();
	}

	deinitialize();
}

void Gameplay::set_level(Level * l) {
	if(!game_running) {
		level = l;
	}
}

void Gameplay::add_player(Player * p) {
	if(!game_running) {
		players->push_back(p);
	}
}

void Gameplay::reset_game() {
	frame = 0;
	
	paused = false;
	ended = false;
	countdown = true;

	level->reset();
}

void Gameplay::on_pre_processing() {}
void Gameplay::on_post_processing() {}

void Gameplay::process_player_collission() {}
void Gameplay::process_npc_collission() {}

void Gameplay::initialize() {
	// Load graphics, fonts, etc
}

void Gameplay::deinitialize() {
	// Unload graphics, fonts, etc
}
