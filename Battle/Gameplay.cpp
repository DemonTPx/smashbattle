#include "SDL/SDL.h"

#include <vector>

#include "Main.h"
#include "Level.h"
#include "Player.h"
#include "GameplayObject.h"
#include "PauseMenu.h"

#include "Gameplay.h"

#ifndef WIN32
#define sprintf_s snprintf
#endif

int Gameplay::frame = 0;
Gameplay * Gameplay::instance = 0;

Gameplay::Gameplay() {
	instance = this;

	game_running = false;
	players = new std::vector<Player*>();
	npcs = new std::vector<NPC*>();
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

	Main::autoreset = false;

	initialize();

	reset_game();

	game_running = true;
	Main::instance->audio->stop_music();
	music_playing = false;
	
	// Set the input defaults
	for(unsigned int idx = 0; idx < players->size(); idx++) {
		Player * p = players->at(idx);
		p->input->unset_delay();
	}

	while(Main::running && game_running) {
		// Event handling
		while(SDL_PollEvent(&event)) {
			Main::instance->handle_event(&event);

			// Handle player input
			for(unsigned int idx = 0; idx < players->size(); idx++) {
				Player * p = players->at(idx);
				p->input->handle_event(&event);
			}

			handle_pause_input(&event);

		}

		// Gameplay processing
		on_pre_processing();
		if(!countdown && !ended) {
			// Move and process players
			for(unsigned int idx = 0; idx < players->size(); idx++) {
				Player * p = players->at(idx);
				p->move(level);
				p->process();
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
			for(unsigned int idx = 0; idx < objects->size(); idx++) {
				GameplayObject * obj = objects->at(idx);
				obj->move(level);
				obj->process();
				for(unsigned int i = 0; i < players->size(); i++) {
					Player * p = players->at(i);
					if(p->is_dead)
						continue;
					SDL_Rect * rect;
					rect = p->get_rect();
					if(is_intersecting(rect, obj->position)) {
						obj->hit_player(p);
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
				}
			}
		}
		if(countdown) {
			process_countdown();
		}

		on_post_processing();

		// Drawing
		frame++;

		level->draw(screen);

		for(unsigned int idx = 0; idx < players->size(); idx++) {
			Player * p = players->at(idx);
			if(countdown)
				p->draw(screen, true);
			else
				p->draw(screen);
		}
		for(unsigned int idx = 0; idx < npcs->size(); idx++) {
			NPC * npc = npcs->at(idx);
			npc->draw(screen);
		}

		for(unsigned int idx = 0; idx < objects->size(); idx++) {
			GameplayObject * obj = objects->at(idx);
			obj->draw(screen);
		}

		draw_score();
		
		if(ended) {
			draw_game_ended();

			if(frame - end_start > 120) {
				reset_game();
			}
		}
		if(countdown) {
			draw_countdown();
		}

		Main::instance->flip();
	}

	deinitialize();

	Main::autoreset = true;
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

void Gameplay::add_npc(NPC * npc) {
	npcs->push_back(npc);
}

void Gameplay::bounce_up_players_and_npcs(SDL_Rect * rect, SDL_Rect * source) {
	Player * p;
	for(unsigned int i = 0; i < players->size(); i++) {
		p = players->at(i);
		if(is_intersecting(rect, p->position))
			p->bounce_up(source);
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

	level->reset();
	
	// Clear gameplay objects
	GameplayObject * obj;
	for(unsigned int i = 0; i < objects->size(); i++) {
		obj = objects->at(i);
		delete obj;
	}
	objects->clear();

	on_game_reset();

	//Main::audio->stop_music();

	srand(SDL_GetTicks());
}

void Gameplay::initialize() {
	pause_menu = new PauseMenu(screen);
	pause_menu->add_option((char*)"RESUME\0");
	pause_menu->add_option((char*)"QUIT\0");
}

void Gameplay::deinitialize() {
	// Stop the music
	if(!countdown)
		Main::audio->stop_music();

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
		surf = Main::text->render_text_medium_shadow(countdown_pre_text);
	} else {
		sprintf_s(text, 5, "%d", countdown_sec_left);
		surf = Main::text->render_text_large_shadow(text);
	}

	SDL_Rect rect;
	rect.x = (screen->w - surf->w) / 2;
	rect.y = (screen->h - surf->h) / 2;
	
	SDL_BlitSurface(surf, NULL, screen, &rect);
	SDL_FreeSurface(surf);
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


void Gameplay::process_player_collission() {
	if(!players_collide)
		return;

	Player * p1, * p2;
	SDL_Rect * r1, * r2;

	for(unsigned int i1 = 0; i1 < players->size(); i1++) {
		p1 = players->at(i1);
		
		if(p1->is_dead) continue;

		for(unsigned int i2 = (i1 + 1); i2 < players->size(); i2++) {
			p2 = players->at(i2);

			if(p2->is_dead) continue;

			r1 = p1->get_rect();
			r2 = p2->get_rect();

			if(is_intersecting(r1, r2)) {
				Main::audio->play(SND_BOUNCE, (p1->position->x + p2->position->x) / 2);

				p1->bounce(p2);
				p2->bounce(p1);

				p1->momentumx = p1->newmomentumx;
				p2->momentumx = p2->newmomentumx;
			}

			delete r1;
			delete r2;
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
			Main::audio->play(SND_GO);
			if(!music_playing) {
				Main::audio->play_music(level->music);
				music_playing = true;
			}
			return;
		}
		countdown_sec_left--;
		countdown_start = frame;

		if(countdown_sec_left <= 3)
			Main::audio->play(SND_COUNTDOWN);
	}
}
