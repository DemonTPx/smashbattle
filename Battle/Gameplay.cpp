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
Gameplay * Gameplay::instance = 0;

Gameplay::Gameplay() {
	instance = this;

	game_running = false;
	players = new std::vector<Player*>();
	objects = new std::vector<GameplayObject*>();
	
	players_collide = true;
	npcs_collide = true;
	players_npcs_collide = true;

}

Gameplay::~Gameplay() {
	instance = NULL;
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

			//handle_pause_input(&event);

			// Handle player input
			for(unsigned int idx = 0; idx < players->size(); idx++) {
				Player * p = players->at(idx);
				p->handle_event(&event);
			}
		}

		// Gameplay processing
		on_pre_processing();
		if(!paused && !countdown && !ended) {
			// Move and process players
			for(unsigned int idx = 0; idx < players->size(); idx++) {
				Player * p = players->at(idx);
				p->move(level);
				p->process();
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
				obj->move(level);
				obj->process();
				for(unsigned int i = 0; i < players->size(); i++) {
					Player * p = players->at(i);
					SDL_Rect * rect;
					rect = p->get_rect();
					if(is_intersecting(rect, obj->position)) {
						obj->hit_player(p);
					}
					delete rect;
				}
				if(obj->done) {
					objects->erase(objects->begin() + idx);
					delete obj;
				}
			}
		}
		on_post_processing();

		// Drawing
		if(!paused)
			frame++;

		level->draw(screen);

		for(unsigned int idx = 0; idx < players->size(); idx++) {
			Player * p = players->at(idx);
			p->draw(screen);
		}
	/*	for(unsigned int idx = 0; idx < npcs->size(); idx++) {
			NPC * npc = npcs->at(idx);
			npc->draw(screen);
		}*/

		for(unsigned int idx = 0; idx < objects->size(); idx++) {
			GameplayObject * obj = objects->at(idx);
			obj->draw(screen);
		}

		
		if(ended) {
			draw_game_ended();
			
			if(paused && !end_timer->is_paused()) end_timer->pause();
			if(!paused && end_timer->is_paused()) end_timer->unpause();

			if(end_timer->get_ticks() > 2000) {
				delete end_timer;
				reset_game();
			}
		}
		if(countdown) {
			if(!paused)
				draw_countdown();
		}
		if(paused) {
			draw_pause_screen();
		}

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

void Gameplay::bounce_up_players_and_npcs(SDL_Rect * rect) {
	Player * p;
	for(unsigned int i = 0; i < players->size(); i++) {
		p = players->at(i);
		if(is_intersecting(rect, p->position))
			p->bounce_up();
	}

	/*
	NPC * npc;
	for(unsigned int i = 0; i < npcs->size(); i++) {
		npc = npcs->at(i);
		if(is_intersecting(rect, npc->position))
			npc->bounce_up();
	}*/
}

void Gameplay::add_object(GameplayObject * obj) {
	objects->push_back(obj);
}

void Gameplay::reset_game() {
	frame = 0;
	
	paused = false;
	ended = false;
	//countdown = true;
	countdown = false;
	
	/*countdown_sec_left = 4;
	countdown_timer = new Timer();
	countdown_timer->start();*/

	level->reset();
	
	// Clear gameplay objects
	GameplayObject * obj;
	for(unsigned int i = 0; i < objects->size(); i++) {
		obj = objects->at(i);
		delete obj;
	}
	objects->clear();

	on_game_reset();

	Main::audio->stop_music();
}

void Gameplay::initialize() {
	// Load graphics, fonts, etc
}

void Gameplay::deinitialize() {
	// Stop the music
	if(!countdown)
		Main::audio->stop_music();

	// Clear players (we don't delete the players, because they are not created in this class
	players->clear();
	delete players;

	// Delete timers
	if(ended)
		delete end_timer;
	if(countdown)
		delete countdown_timer;

	// Clear gameplay objects
	GameplayObject * obj;
	for(unsigned int i = 0; i < objects->size(); i++) {
		obj = objects->at(i);
		delete obj;
	}
	objects->clear();
	delete objects;

	// Unload graphics, fonts, etc
}

void Gameplay::draw_countdown() {

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
	if(l1 > r2) intersect = false;
	if(r1 < l2) intersect = false;
	if(t1 > b2) intersect = false;
	if(b1 < t2) intersect = false;
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


void Gameplay::process_player_collission() {
	if(!players_collide)
		return;

	Player * p1, * p2;
	SDL_Rect * r1, * r2;

	for(unsigned int i1 = 0; i1 < players->size(); i1++) {
		p1 = players->at(i1);
		for(unsigned int i2 = (i1 + 1); i2 < players->size(); i2++) {
			p2 = players->at(i2);
			r1 = p1->get_rect();
			r2 = p2->get_rect();

			if(is_intersecting(r1, r2)) {
				Main::audio->play(SND_BOUNCE);

				// Move players out of collision zone

				p1->bounce(r2);
				p2->bounce(r1);
			}

			delete r1;
			delete r2;
		}
	}
}

void Gameplay::process_npc_collission() {}
