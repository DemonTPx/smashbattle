#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"

#include <vector>
#include <iostream>
#include <fstream>

#include "Timer.h"
#include "AudioController.h"
#include "Main.h"

#include "CharacterSelect.h"

#include "Player.h"
#include "Projectile.h"
#include "PowerUp.h"
#include "AmmoPowerUp.h"
#include "HealthPowerUp.h"
#include "BombPowerUp.h"

#include "Battle.h"

#ifndef WIN32
#define sprintf_s snprintf
#endif

#define SPR_W 32
#define SPR_H 32
#define SPR_COLS 20
#define SPR_ROWS 15
#define SPR_COUNT (20 * 15)

#define MAX_MOMENTUM_FALL 100
#define MAX_MOMENTUM_JUMP 40
#define MAX_MOMENTUM_HORIZ 20
#define MAX_MOMENTUM_HORIZ_DUCKJUMP 10
#define MAX_MOMENTUM_RUN 40

#define FRAME_CYCLE_DISTANCE 24

#define MOMENTUM_INTERV_HORIZ 1
#define MOMENTUM_INTERV_VERT 1

#define SPEED_HORIZ 2
#define SPEED_VERT 2

const int Battle::CHARACTER_COUNT = 8;
const Character Battle::characters[Battle::CHARACTER_COUNT] = {
	{(char*)"Bert", (char*)"gfx/bert.bmp"},
	{(char*)"Jeroen", (char*)"gfx/jeroen.bmp"},
	{(char*)"Steven", (char*)"gfx/steven.bmp"},
	{(char*)"Tedje", (char*)"gfx/tedje.bmp"},
	{(char*)"Okke", (char*)"gfx/okke.bmp"},
	{(char*)"Jeremy", (char*)"gfx/jeremy.bmp"},
	{(char*)"Marcel", (char*)"gfx/marcel.bmp"},
	{(char*)"Anton", (char*)"gfx/anton.bmp"},
};
const int Battle::STAGE_COUNT = 4;
const Stage Battle::stages[Battle::STAGE_COUNT] = {
	{(char*)"Battle Arena one", (char*)"Bert Hekman", (char*)"stage/one.stg"},
	{(char*)"Battle Arena two", (char*)"Bert Hekman", (char*)"stage/two.stg"},
	{(char*)"Battle Arena three", (char*)"Bert Hekman", (char*)"stage/three.stg"},
	{(char*)"Battle Arena four", (char*)"Bert Hekman", (char*)"stage/four.stg"},
};

Battle::Battle() {

}

Battle::~Battle() {

}

void Battle::run() {
	load_images();

	SDL_Event event;
	SDL_Surface * screen;

	screen = Main::instance->screen;

	game_running = true;

	CharacterSelect * character_select;
	character_select = new CharacterSelect();
	character_select->run();

	player1 = new Player(character_select->name1, 1, character_select->file1);
	player1->controls = Main::instance->controls1;

	player2 = new Player(character_select->name2, 2, character_select->file2);
	player2->controls = Main::instance->controls2;

	load_level(stages[character_select->stage].filename);

	delete character_select;

	projectiles = new std::vector<Projectile*>(0);
	bombs = new std::vector<Bomb*>(0);
	powerups = new std::vector<PowerUp*>(0);

	bullets_unlimited = true;

	reset_game();

	while (Main::running && game_running) {
		// Event handling
		while(SDL_PollEvent(&event)) {
			Main::instance->handle_event(&event);
			handle_pause_input(&event);
			player1->handle_input(&event);
			player2->handle_input(&event);
		}

		// Processing
		if(!paused && !countdown && !ended) {

			move_player(player1);
			move_player(player2);

			process_shoot(player1);
			process_shoot(player2);

			for(unsigned int idx = 0; idx < projectiles->size(); idx++) {
				Projectile * p = projectiles->at(idx);
				move_projectile(p);
				if(p->hit) {
					projectiles->erase(projectiles->begin() + idx);
					delete p;
				}
			}

			for(unsigned int idx = 0; idx < bombs->size(); idx++) {
				Bomb * b = bombs->at(idx);
				move_bomb(b);
				if(b->done) {
					bombs->erase(bombs->begin() + idx);
					delete b;
				}
			}

			check_player_collision(player1, player2);

			check_player_projectile_collision(player1);
			check_player_projectile_collision(player2);

			check_player_bomb_collision(player1);
			check_player_bomb_collision(player2);

			check_player_powerup_collision(player1);
			check_player_powerup_collision(player2);

			generate_powerup(false);

			// Players die when they fall into the pit :)
			if(player1->position->y + player1->position->h > 14 * SPR_H) {
				player1->hitpoints = 0;
			}
			if(player2->position->y + player2->position->h > 14 * SPR_H) {
				player2->hitpoints = 0;
			}
			
			// Check if any player is out of HP
			if(player1->hitpoints <= 0) {
				Main::audio->play(SND_YOULOSE);

				if(player1->hitpoints < 0)
					player1->hitpoints = 0;
				player2->score++;

				player1->is_hit = true;
				player2->is_hit = false;

				ended = true;
				end_timer = new Timer();
				end_timer->start();
			}
			if(player2->hitpoints <= 0) {
				Main::audio->play(SND_YOULOSE);

				if(player2->hitpoints < 0)
					player2->hitpoints = 0;
				player1->score++;

				player1->is_hit = false;
				player2->is_hit = true;

				ended = true;
				end_timer = new Timer();
				end_timer->start();
			}
		}
		
		// Drawing

		frame++;
		draw_level(screen);

		player1->show(screen);
		player2->show(screen);

		draw_score(screen);

		for(unsigned int idx = 0; idx < powerups->size(); idx++) {
			PowerUp * p = powerups->at(idx);
			p->show(screen);
		}

		for(unsigned int idx = 0; idx < bombs->size(); idx++) {
			Bomb * b = bombs->at(idx);
			b->show(screen);
		}

		for(unsigned int idx = 0; idx < projectiles->size(); idx++) {
			Projectile * p = projectiles->at(idx);
			p->show(screen);
		}
		
		if(paused) {
			draw_pause_screen(screen);
		}
		if(countdown) {
			if(!paused)
				handle_draw_countdown(screen);
		}
		if(ended) {
			draw_win_screen(screen);
			if(end_timer->get_ticks() > 2000) {
				delete end_timer;
				reset_game();
			}
		}

		// Flipping

		Main::instance->flip();
	}

	if(!countdown)
		Main::audio->stop_music();

	delete player1;
	delete player2;

	if(ended)
		delete end_timer;
	if(countdown)
		delete countdown_timer;

	// Clear vectors
	Projectile * pr;
	for(unsigned int i = 0; i < projectiles->size(); i++) {
		pr = projectiles->at(i);
		delete pr;
	}
	projectiles->clear();
	delete projectiles;

	Bomb * b;
	for(unsigned int i = 0; i < bombs->size(); i++) {
		b = bombs->at(i);
		delete b;
	}
	bombs->clear();
	delete bombs;

	PowerUp * pu;
	for(unsigned int i = 0; i < powerups->size(); i++) {
		pu = powerups->at(i);
		pu->cleanup();
		delete pu;
	}
	powerups->clear();
	delete powerups;

	free_images();
}

void Battle::reset_game() {
	// Reset player 1
	player1->position->x = 160 + ((SPR_W - player1->position->w) / 2);
	player1->position->y = 320 - player1->position->h;
	player1->hitpoints = 100;
	player1->shoot_start = 0;
	player1->is_duck_forced = false;
	player1->duck_force_start = 0;
	player1->is_hit = false;
	player1->hit_start = 0;
	player1->bullets = 20;
	player1->bombs = 3;
	player1->is_falling = false;
	player1->is_jumping = false;
	player1->momentumx = 0;
	player1->momentumy = 0;
	player1->set_sprite(SPR_R);

	// Reset player 2
	player2->position->x = 448 + ((SPR_W - player2->position->w) / 2);
	player2->position->y = 320 - player2->position->h;
	player2->hitpoints = 100;
	player2->shoot_start = 0;
	player2->is_duck_forced = false;
	player2->duck_force_start = 0;
	player2->is_hit = false;
	player2->hit_start = 0;
	player2->bullets = 20;
	player2->bombs = 3;
	player2->is_falling = false;
	player2->is_jumping = false;
	player2->momentumx = 0;
	player2->momentumy = 0;
	player2->set_sprite(SPR_L);

	srand(SDL_GetTicks());

	Projectile * pr;
	for(unsigned int i = 0; i < projectiles->size(); i++) {
		pr = projectiles->at(i);
		delete pr;
	}

	projectiles->clear();

	Bomb * b;
	for(unsigned int i = 0; i < bombs->size(); i++) {
		b = bombs->at(i);
		delete b;
	}

	bombs->clear();

	PowerUp * pu;
	for(unsigned int i = 0; i < powerups->size(); i++) {
		pu = powerups->at(i);
		delete pu;
	}

	powerups->clear();

	frame = 0;
	paused = false;
	ended = false;
	countdown = true;
	countdown_sec_left = 4;
	countdown_timer = new Timer();
	countdown_timer->start();

	Main::audio->stop_music();
}

void Battle::handle_pause_input(SDL_Event * event) {
	if(paused) {
		if(event->type == SDL_KEYDOWN) {
			if(event->key.keysym.sym == pause_player->controls.kb_down ||
				event->key.keysym.sym == pause_player->controls.kb_up) {
					pause_quit = !pause_quit;
			}
			if(event->key.keysym.sym == pause_player->controls.kb_shoot ||
				event->key.keysym.sym == pause_player->controls.kb_run ||
				(pause_player->controls.kb_jump != pause_player->controls.kb_up &&
				event->key.keysym.sym == pause_player->controls.kb_jump)) {
					if(pause_quit) {
						game_running = false;
					} else {
						paused = false;
						Main::instance->audio->unpause_music();
						if(countdown) countdown_timer->unpause();
					}
			}
			if(event->key.keysym.sym == pause_player->controls.kb_start) {
				paused = false;
				Main::instance->audio->unpause_music();
				if(countdown) countdown_timer->unpause();
			}
		}
		if(event->type == SDL_JOYAXISMOTION) {
			if(event->jaxis.which == pause_player->controls.joystick_idx && event->jaxis.axis == 1) {
				if(event->jaxis.value < -6400 || event->jaxis.value > 6400) {
					pause_quit = !pause_quit;
				}
			}
		}
		if(event->type == SDL_JOYBUTTONDOWN) {
			if(event->jbutton.which == pause_player->controls.joystick_idx &&
				(event->jbutton.button == pause_player->controls.js_run ||
				event->jbutton.button == pause_player->controls.js_jump ||
				event->jbutton.button == pause_player->controls.js_shoot)) {
					if(pause_quit) {
						game_running = false;
					} else {
						paused = false;
						Main::instance->audio->unpause_music();
						if(countdown) countdown_timer->unpause();
					}
			}
		}
	} else {
		if(event->type == SDL_KEYDOWN) {
			if(event->key.keysym.sym == player1->controls.kb_start || event->key.keysym.sym == player2->controls.kb_start) {
				if(!(paused && event->key.keysym.sym != pause_player->controls.kb_start)) {
					paused = true;
					if(event->key.keysym.sym == player1->controls.kb_start)
						pause_player = player1;
					else if(event->key.keysym.sym == player2->controls.kb_start)
						pause_player = player2;
					pause_quit = false;
					Main::instance->audio->pause_music();
					if(countdown) countdown_timer->pause();
					Main::instance->audio->play(SND_PAUSE);
				}
			}
		}
		if(event->type == SDL_JOYBUTTONDOWN) {
			if((player1->controls.use_joystick &&
				event->jbutton.which == player1->controls.joystick_idx &&
				event->jbutton.button == player1->controls.js_start) ||
				(player2->controls.use_joystick &&
				event->jbutton.which == player2->controls.joystick_idx &&
				event->jbutton.button == player2->controls.js_start)) {
					paused = true;
					if(event->key.keysym.sym == player1->controls.kb_start)
						pause_player = player1;
					else if(event->key.keysym.sym == player2->controls.kb_start)
						pause_player = player2;
					pause_quit = false;
					Main::instance->audio->pause_music();
					if(countdown) countdown_timer->pause();
					Main::instance->audio->play(SND_PAUSE);
			}
		}
	}
}

void Battle::process_shoot(Player * p) {
	if(p->keydn_shoot) {
		if(frame > p->shoot_start + p->shoot_delay && (bullets_unlimited ||  p->bullets > 0)) {
			p->shoot_start = frame;
			Projectile * pr;
			SDL_Rect * clip_weapon;

			clip_weapon = new SDL_Rect();
			clip_weapon->x = 0;
			clip_weapon->y = 0;
			clip_weapon->w = 8;
			clip_weapon->h = 8;

			pr = new Projectile(weapons, clip_weapon);
			pr->damage = 10;

			if(p->current_sprite >= SPR_L && p->current_sprite <= SPR_L_DUCK) {
				pr->speedx = -10;
				pr->position->x = p->position->x - pr->position->w - 1;
			} else {
				pr->speedx = 10;
				pr->position->x = p->position->x + p->position->w + 1;
			}
			if(p->is_duck)
				pr->position->y = p->position->y + 28;
			else
				pr->position->y = p->position->y + 8;
			projectiles->push_back(pr);

			if(!bullets_unlimited)
				p->bullets -= 1;

			Main::instance->audio->play(SND_SHOOT);
		}
	}
	if(p->keydn_bomb) {
		if(frame > p->shoot_start + p->shoot_delay && p->bombs > 0) {
			p->shoot_start = frame;
			Bomb * b;

			b = new Bomb(surface_bombs);
			b->damage = 25;
			b->time = 60;
			b->frame_start = frame;
			b->frame_change_start = frame;
			b->frame_change_count = 12;
			b->owner = p;
			b->position->x = p->position->x + (p->position->w - p->position->w) / 2;
			b->position->y = p->position->y + (p->position->h - b->position->h);
			bombs->push_back(b);

			p->bombs -= 1;

			Main::instance->audio->play(SND_SHOOT);
		}
	}
}

void Battle::generate_powerup(bool force) {
	int r;
	int row, col;
	if(!force) {
		r = rand();
		if(r % 500 != 0) return;
		if(powerups->size() >= 2) return;
	}
	PowerUp * pu;
	SDL_Rect * rect, * pos;

	bool done;
	done = false;
	while(!done) {
		row = rand() % SPR_ROWS - 1;
		col = rand() % SPR_COLS;
		if(level[row * SPR_COLS + col] == -1 && level[(row + 1) * SPR_COLS + col] != -1) {
			done = true;
			for(unsigned int idx = 0; idx < powerups->size(); idx++) {
				if(powerups->at(idx)->position->x == (col * SPR_W) + 8 &&
					powerups->at(idx)->position->y == (row * SPR_H) + 16) {
					done = false;
				}
			}
		}
	}

	pos = new SDL_Rect();
	pos->w = 16;
	pos->h = 16;
	pos->x = (col * SPR_W) + 8;
	pos->y = (row * SPR_H) + 16;
	
	r = rand() % 4;
	switch(r) {
		case 0:
			if(!bullets_unlimited) {
				rect = new SDL_Rect();
				rect->x = 16;
				rect->y = 0;
				rect->w = 16;
				rect->h = 16;
				pu = new AmmoPowerUp(powerup, rect, pos, 20);
				break;
			}
		case 1:
		case 2:
			rect = new SDL_Rect();
			rect->x = 0;
			rect->y = 0;
			rect->w = 16;
			rect->h = 16;
			pu = new HealthPowerUp(powerup, rect, pos, 25);
			break;
		case 3:
			rect = new SDL_Rect();
			rect->x = 16;
			rect->y = 0;
			rect->w = 16;
			rect->h = 16;
			pu = new BombPowerUp(powerup, rect, pos, 1);
			break;
		default:
			delete pos;
			pu = NULL;
			break;
	}

	if(pu != NULL)
		powerups->push_back(pu);
}

void Battle::move_player(Player * p) {
	int speedx, speedy;
	int maxx;
	int momentumx_old;
	SDL_Rect rect;
	
	speedx = 0;
	speedy = 0;
	
	if(p->is_hit) {
		// The player has been hit long enough
		if(frame > p->hit_start + p->hit_delay) {
			p->is_hit = false;
		}
	}

	speedx = SPEED_HORIZ;

	momentumx_old = p->momentumx;
	
	// Are we running?
	if(p->keydn_run) {
		p->is_running = true;
		maxx = MAX_MOMENTUM_RUN;
	} else {
		p->is_running = false;
		maxx = MAX_MOMENTUM_HORIZ;
	}
	
	// Are we ducking?
	if(p->keydn_d) {
		p->is_duck = true;
	}
	else {
		p->is_duck = false;
	}
	
	// Are we forced to being ducked?
	if(p->is_duck_forced) {
		if(frame - p->duck_force_start > DUCK_FORCE_FRAMES) {
			p->is_duck_forced = false;
		} else {
			p->is_duck = true;
		}
	}

	// Force the player to duck when bumping the head
	if(!p->is_duck && check_collision(p->position)) {
		p->is_duck = true;
	}

	if(p->is_duck) {
		if(p->is_jumping || p->is_falling) {
			// The player can move when jumping and ducking at the same time
			maxx = MAX_MOMENTUM_HORIZ_DUCKJUMP;
		} else {
			// The player cannot move when ducking and standing on the ground
			maxx = 0;
		}
	}

	if(p->keydn_l) {
		// Move more to the left
		if(p->momentumx > 0) p->momentumx -= MOMENTUM_INTERV_HORIZ;
		if(p->momentumx >= -maxx) p->momentumx -= MOMENTUM_INTERV_HORIZ;
		else p->momentumx += MOMENTUM_INTERV_HORIZ;
	}
	if(p->keydn_r) {
		// Move more to the right
		if(p->momentumx < 0) p->momentumx += MOMENTUM_INTERV_HORIZ;
		if(p->momentumx <= maxx) p->momentumx += MOMENTUM_INTERV_HORIZ;
		else p->momentumx -= MOMENTUM_INTERV_HORIZ;
	}
	if(!p->keydn_l && !p->keydn_r) {
		// Slide until we're standing still
		if(p->momentumx < 0) p->momentumx += MOMENTUM_INTERV_HORIZ;
		if(p->momentumx > 0) p->momentumx -= MOMENTUM_INTERV_HORIZ;
	}

	// Move the player horizontally
	speedx = (int)((double)speedx * ((double)p->momentumx / 10));
	p->position->x += speedx;
	p->last_speedx = speedx;

	// Which sprite do we want to show?
	if(p->momentumx == 0) {
		// Standing still
		if(!p->keydn_l && !p->keydn_r) {
			if(p->current_sprite >= SPR_L && p->current_sprite <= SPR_L_DUCK) {
				if(p->is_duck) {
					p->set_sprite(SPR_L_DUCK);
				} else if(p->is_jumping || p->is_falling) {
					p->set_sprite(SPR_L_JUMP);
				} else {
					p->set_sprite(SPR_L);
				}
			}
			if(p->current_sprite >= SPR_R && p->current_sprite <= SPR_R_DUCK) {
				if(p->is_duck) {
					p->set_sprite(SPR_R_DUCK);
				} else if(p->is_jumping || p->is_falling) {
					p->set_sprite(SPR_R_JUMP);
				} else {
					p->set_sprite(SPR_R);
				}
			}/*
			if(p->current_sprite != SPR_L || p->current_sprite != SPR_R) {
				p->set_sprite(SPR_L);
			}*/
		}
		else {
			if(p->keydn_l && !p->keydn_r) {
				if(p->is_duck)
					p->set_sprite(SPR_L_DUCK);
				else
					p->set_sprite(SPR_L_WALK1);
			}
			if(!p->keydn_l && p->keydn_r) {
				if(p->is_duck)
					p->set_sprite(SPR_R_DUCK);
				else
					p->set_sprite(SPR_R_WALK1);
			}
		}
		p->distance_walked = 0;
	}
	if(p->momentumx < 0) {
		// Moving left
		if(p->is_duck) {
			p->set_sprite(SPR_L_DUCK);
		} else if((p->is_jumping || p->is_falling) && !p->is_duck) {
			p->set_sprite(SPR_L_JUMP);
		} else {
			if(p->is_running) {
				if(p->current_sprite < SPR_L_RUN1 || p->current_sprite > SPR_L_RUN3) {
					p->set_sprite(SPR_L_RUN1);
				}
			} else {
				if(p->current_sprite < SPR_L_WALK1 || p->current_sprite > SPR_L_WALK3) {
					p->set_sprite(SPR_L_WALK1);
				}
			}
			if(p->keydn_r) {
				p->set_sprite(SPR_L_BRAKE);
				p->distance_walked = 0;
			}
			if(p->distance_walked < -FRAME_CYCLE_DISTANCE) {
				if(p->is_running)
					p->cycle_sprite(SPR_L_RUN1, SPR_L_RUN3);
				else
					p->cycle_sprite(SPR_L_WALK1, SPR_L_WALK3);
				p->distance_walked = 0;
			}
			p->distance_walked += speedx;
		}
	}
	else if(p->momentumx > 0) {
		// Moving right
		if(p->is_duck) {
			p->set_sprite(SPR_R_DUCK);
		} else if(p->is_jumping || p->is_falling) {
			p->set_sprite(SPR_R_JUMP);
		} else {
			if(p->is_running) {
				if(p->current_sprite < SPR_R_RUN1 || p->current_sprite > SPR_R_RUN3) {
					p->set_sprite(SPR_R_RUN1);
				}
			} else {
				if(p->current_sprite < SPR_R_WALK1 || p->current_sprite > SPR_R_WALK3) {
					p->set_sprite(SPR_R_WALK1);
				}
			}
			if(p->keydn_l) {
				p->set_sprite(SPR_R_BRAKE);
				p->distance_walked = 0;
			}
			if(p->distance_walked > FRAME_CYCLE_DISTANCE) {
				if(p->is_running)
					p->cycle_sprite(SPR_R_RUN1, SPR_R_RUN3);
				else
					p->cycle_sprite(SPR_R_WALK1, SPR_R_WALK3);
				p->distance_walked = 0;
			}
			p->distance_walked += speedx;
		}
	}

	rect.x = p->position->x;
	rect.y = p->position->y;
	rect.w = p->position->w;
	rect.h = p->position->h;
	if(p->is_duck) {
		// If the player is ducking, the top should be lower
		rect.y = p->position->y + (PLAYER_H - PLAYER_DUCK_H);
		rect.h = PLAYER_DUCK_H;
	}
	
	if(check_collision(&rect)) {
		// Stop if colliding with the level
		p->position->x -= speedx;
		p->momentumx = 0;
	}

	// Move through the sides
	// If we went too far to the right, appear at the far left (and vica versa)
	if(p->position->x >= WINDOW_WIDTH)
		p->position->x -= WINDOW_WIDTH;
	if(p->position->x <= 0)
		p->position->x += WINDOW_WIDTH;
	

	// Jumping

	if(p->keydn_u && !p->is_falling && !p->is_jumping) {
		// Start the jump
		p->momentumy = MAX_MOMENTUM_JUMP;
		p->is_jumping = true;

		Main::instance->audio->play(SND_JUMP);
	}
	if(!p->keydn_u && p->is_jumping) {
		// The up key is released, so fall faster
		p->is_jumping = false;
		p->is_falling = true;
	}
	if(p->is_falling || p->is_jumping) {
		speedy = SPEED_VERT;
		// Increase downward momentum (= decrease upward momentum)
		if(p->momentumy > -MAX_MOMENTUM_FALL) {
			p->momentumy -= MOMENTUM_INTERV_VERT;
		// Falling is faster than jumping (also.. we start to fall faster when the
		// up key is not held down)
			if(p->is_falling)
				p->momentumy  -= MOMENTUM_INTERV_VERT;
		}
	}

	speedy = (int)((double)speedy * ((double)p->momentumy / 10));
	
	// Move the player vertically
	p->position->y -= speedy;
	p->last_speedy = speedy;

	rect.x = p->position->x;
	rect.y = p->position->y;
	rect.w = p->position->w;
	rect.h = p->position->h;
	if(p->is_duck) {
		// If the player is ducking, the top should be lower
		rect.y = p->position->y + (PLAYER_H - PLAYER_DUCK_H);
		rect.h = PLAYER_DUCK_H;
	}

	// Did we hit something?
	if(check_collision(&rect)) {
		p->position->y += speedy;

		if(p->is_jumping) {
			// Bounce off the top (bump head)
			p->is_jumping = false;
			p->is_falling = true;
			p->momentumy = 0;
		} else {
			// Stop at the bottom
			p->is_jumping = false;
			p->is_falling = false;
			p->momentumy = 0;
		}
	}

	if(!p->is_jumping && !p->is_falling && check_bottom(p->position)) {
		// start falling when there is no bottom
		p->is_falling = true;
	}
}

void Battle::move_projectile(Projectile * p) {
	p->position->x += p->speedx;
	p->distance_traveled += p->speedx;
	if(p->position->x < 0)
		p->position->x += WINDOW_WIDTH;
	if(p->position->x > WINDOW_WIDTH)
		p->position->x -= WINDOW_WIDTH;

	if(check_collision(p->position)) {
		p->speedx = 0;
		p->hit = true;
	}

	p->position->y += p->speedy;
	p->distance_traveled += p->speedy;
	/*
	if(p->position->y < 0)
		p->position->y += WINDOW_HEIGHT;
	if(p->position->y > WINDOW_HEIGHT)
		p->position->y -= WINDOW_HEIGHT;
	*/

	if(check_collision(p->position)) {
		p->speedy = 0;
		p->hit = true;
	}

	if(p->distance_traveled > p->max_distance || p->distance_traveled < -p->max_distance) {
		p->hit = true;
	}
}

void Battle::move_bomb(Bomb * b) {
	int speed;

	if(!b->exploded) {
		// Move the bomb
		speed = (int)(b->speedy / 10);

		if(speed != 0) {
			b->position->y += speed;

			if(check_collision(b->position)) {
				b->position->y -= speed;
				b->speedy = 0;
			}
		}
		
		// Check if the bomb is on the floor, fall if not
		if(check_bottom(b->position)) {
			if(b->speedy > -MAX_MOMENTUM_FALL)
				b->speedy += 2;
		}

		// Animate bomb
		if(frame - b->frame_change_start >= b->frame_change_count) {
			b->current_frame = b->current_frame == b->FRAME_NORMAL ? b->FRAME_FLASH : b->FRAME_NORMAL;
			b->frame_change_start = frame;
		}

		// Explode
		if(frame - b->frame_start >= b->time) {
			b->exploded = true;

			Main::audio->play(SND_EXPLODE);
			
			Player * other;
			SDL_Rect * rect;
			other = b->owner == player1 ? player2 : player1;
			rect = b->get_damage_rect();
			if(is_intersecting(other->position, rect)) {
				other->hitpoints -= b->damage;
				other->is_hit = true;
				other->hit_start = frame;
			}
			delete rect;
		}
	}
}

void Battle::check_player_collision(Player * p1, Player * p2) {
	// Check if there is a collision between the players and process the further movement
	SDL_Rect * rect1, * rect2;
	bool intersecting;
	int l1, r1, t1, b1;
	int l2, r2, t2, b2;
	int momxl, momyl;
	int momxr, momyr;
	int diffx, diffy;
	Player * upper, * lower;
	Player * left, * right;

	// Return if not colliding
	rect1 = player1->get_rect();
	rect2 = player2->get_rect();
	intersecting = is_intersecting(rect1, rect2);
	if(!intersecting) {
		delete rect1;
		delete rect2;
		return;
	}

	// Collision
	l1 = rect1->x;
	t1 = rect1->y;
	r1 = rect1->x + rect1->w;
	b1 = rect1->y + rect1->h;

	l2 = rect2->x;
	t2 = rect2->y;
	r2 = rect2->x + rect2->w;
	b2 = rect2->y + rect2->h;

	Main::audio->play(SND_BOUNCE);

	// Move players out of collision zone
	p1->position->x -= p1->last_speedx;
	p1->position->y += p1->last_speedy;

	p2->position->x -= p2->last_speedx;
	p2->position->y += p2->last_speedy;

	// Bounce back
	if(t1 < t2) { // p1 above p2
		diffy = b1 - t2;
		upper = p1;
		lower = p2;
	} else { // p1 below p2, or at the same level
		diffy = b2 - t1;
		upper = p2;
		lower = p1;
	}
	
	bool p1_clip = false;
	bool p2_clip = false;
	
	if(r1 >= WINDOW_WIDTH) {
		p1_clip = true;
	}
	if(r2 >= WINDOW_WIDTH) {
		p2_clip = true;
	}

	if(p1->momentumx > p2->momentumx) { // p1 at the left of p2
		diffx = r1 - l2;
		if((p1_clip && !p2_clip) || !p1_clip && p2_clip) {
			diffx -= WINDOW_WIDTH;
		}
		if(p1_clip) diffx -= WINDOW_WIDTH;
		left = p1;
		right = p2;

		momxl = p1->momentumx;
		momyl = p1->momentumy;

		momxr = p2->momentumx;
		momyr = p2->momentumy;
	} else { // p1 at the right of p2, or at the same level
		diffx = r2 - l1;
		if((p1_clip && !p2_clip) || !p1_clip && p2_clip) {
			diffx -= WINDOW_WIDTH;
		}
		left = p2;
		right = p1;

		momxl = p2->momentumx;
		momyl = p2->momentumy;

		momxr = p1->momentumx;
		momyr = p1->momentumy;
	}

	if(diffx > diffy) { // Players hit each others top
		if(upper->keydn_u) {
			Main::audio->play(SND_JUMP);
			upper->momentumy = MAX_MOMENTUM_JUMP;
			upper->is_falling = false;
			upper->is_jumping = true;
		} else {
			upper->momentumy = 30;
		}
		lower->is_duck_forced = true;
		lower->duck_force_start = frame;
		lower->momentumy = -10;
		lower->hitpoints -= 5;
	} else { // Players hit each others side
		if(momxr < -(MAX_MOMENTUM_HORIZ + MOMENTUM_INTERV_HORIZ))
			left->momentumx = -40;
		else
			left->momentumx = -20;
		if(momxl > (MAX_MOMENTUM_HORIZ + MOMENTUM_INTERV_HORIZ))
			right->momentumx = 40;
		else
			right->momentumx = 20;
		upper->momentumy = lower->momentumy;
	}
	
	delete rect1;
	delete rect2;
}

void Battle::check_player_projectile_collision(Player * p) {
	Projectile * pr;
	SDL_Rect * rect;

	if(p->is_hit) return;
	
	rect = p->get_rect();

	for(unsigned int idx = 0; idx < projectiles->size(); idx++) {
		pr = projectiles->at(idx);
		if(is_intersecting(rect, pr->position)) {
			pr->hit = true;
			p->is_hit = true;
			p->hit_start = frame;
			p->hitpoints -= pr->damage;

			Main::audio->play(SND_HIT);
		}
	}

	delete rect;
}

void Battle::check_player_bomb_collision(Player * p) {
	Bomb * b;
	SDL_Rect * rect;

	if(p->is_hit) return;
	
	rect = p->get_rect();

	for(unsigned int idx = 0; idx < bombs->size(); idx++) {
		b = bombs->at(idx);
		if(!b->exploded && p != b->owner && is_intersecting(rect, b->position)) {
			b->exploded = true;
			Main::audio->play(SND_EXPLODE);

			p->is_hit = true;
			p->hit_start = frame;
			p->hitpoints -= b->damage;
			Main::audio->play(SND_HIT);
		}
	}

	delete rect;
}

void Battle::check_player_powerup_collision(Player * p) {
	PowerUp * pu;
	SDL_Rect * rect;
	rect = p->get_rect();

	for(unsigned int idx = 0; idx < powerups->size(); idx++) {
		pu = powerups->at(idx);

		if(is_intersecting(rect, pu->position)) {
			pu->got_powerup(p);
			powerups->erase(powerups->begin() + idx);
			delete pu;

			Main::audio->play(SND_ITEM);
		}
	}

	delete rect;
}

bool Battle::check_collision(SDL_Rect * rect) {
	// Check if the rect is colliding with the level
	int l, r, t, b;

	l = rect->x;
	r = rect->x + rect->w - 1;

	t = rect->y;
	b = rect->y + rect->h - 1;

	if(t < 0) t = 0;
	if(b < 0) b = 0;
	if(t >= WINDOW_HEIGHT) t = WINDOW_HEIGHT - 1;
	if(b >= WINDOW_HEIGHT) b = WINDOW_HEIGHT - 1;


	if(r >= WINDOW_WIDTH) {
		for(int x = 0; x < r - WINDOW_WIDTH; x++) {
			if(level[level_pos(x, t)] != -1)
				return true;
			if(level[level_pos(x, b)] != -1)
				return true;
		}
		
		r = WINDOW_WIDTH - 1;
	}

	for(int x = l; x < r; x++) {
		if(r > WINDOW_WIDTH)
			break;

		if(level[level_pos(x, t)] != -1)
			return true;
		if(level[level_pos(x, b)] != -1)
			return true;
	}

	if(l >= WINDOW_WIDTH) l -= WINDOW_WIDTH;

	for(int y = t; y < b; y++) {
		if(level[level_pos(l, y)] != -1)
			return true;
		if(level[level_pos(r, y)] != -1)
			return true;
	}

	return false;
}

bool Battle::check_bottom(SDL_Rect * rect) {
	// Check if there is anything to stand on below the rect
	int l, r, t, b;

	l = rect->x;
	r = rect->x + rect->w - 1;

	t = rect->y;
	b = rect->y + rect->h - 1;

	// Above the screen is no bottom
	if(b < 0) return true;

	if(l >= WINDOW_WIDTH) l -= WINDOW_WIDTH;
	if(r >= WINDOW_WIDTH) r -= WINDOW_WIDTH;

	if(level[level_pos(l, b + 1)] != -1)
		return false;
	if(level[level_pos(r, b + 1)] != -1)
		return false;
	return true;
}

bool Battle::is_intersecting(SDL_Rect * one, SDL_Rect * two) {
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

int Battle::level_pos(int x, int y) {
	// Calculate in which grid position the x and y coordinates are
	return ((int)(y / SPR_H) * SPR_COLS) + (int)(x / SPR_W);
}

void Battle::draw_level(SDL_Surface * screen) {
	SDL_Rect rect;

	rect.w = SPR_W;
	rect.h = SPR_H;

	if(background == NULL)
		SDL_FillRect(screen, &screen->clip_rect, 0);
	else
		SDL_BlitSurface(background, NULL, screen, NULL);

	// Draw each sprite, one by one
	for(int i = 0; i < SPR_COUNT; i++) {
		rect.x = (i % SPR_COLS) * SPR_W;
		rect.y = (i / SPR_COLS) * SPR_H;

		// Don't draw empty sprites
		if(level[i] == -1) continue;

		SDL_BlitSurface(tiles, tile_rect[level[i]], screen, &rect);
	}
}

void Battle::draw_score(SDL_Surface * screen) {
	SDL_Surface * surface;
	SDL_Rect rect;
	SDL_Rect rect_s;

	// Health bar player 1
	rect.x = 2;
	rect.y = WINDOW_HEIGHT - 31;
	rect.w = 120;
	rect.h = 9;
	SDL_FillRect(screen, &rect, 0);

	rect_s.w = (int)(1.18 * player1->hitpoints);
	rect_s.h = 7;
	rect_s.x = 0;
	rect_s.y = 0;
	rect.x = 3;
	rect.y = WINDOW_HEIGHT - 30;
	SDL_BlitSurface(player1hp, &rect_s, screen, &rect);

	// Health bar player 2
	rect.x = WINDOW_WIDTH - 122;
	rect.y = WINDOW_HEIGHT - 31;
	rect.w = 120;
	rect.h = 9;
	SDL_FillRect(screen, &rect, 0);

	rect_s.w = (int)(1.18 * player2->hitpoints);
	rect_s.h = 7;
	rect_s.x = 118 - rect_s.w;
	rect_s.y = 0;
	rect.x = WINDOW_WIDTH - 2 - rect_s.w;
	rect.y = WINDOW_HEIGHT - 30;
	SDL_BlitSurface(player2hp, &rect_s, screen, &rect);

	char str[40];

	//sprintf_s(str, 40, "%s %d", player1->name, player1->bullets);
	//surface = TTF_RenderText_Solid(font26, str, fontColor);
	surface = TTF_RenderText_Solid(font26, player1->name, fontColor);
	rect.x = 2;
	rect.y = WINDOW_HEIGHT - surface->h;
	SDL_BlitSurface(surface, NULL, screen, &rect);
	SDL_FreeSurface(surface);

	//sprintf_s(str, 40, "%s %d", player2->name, player2->bullets);
	//surface = TTF_RenderText_Solid(font26, str, fontColor);
	surface = TTF_RenderText_Solid(font26, player2->name, fontColor);
	rect.x = WINDOW_WIDTH - surface->w - 2;
	rect.y = WINDOW_HEIGHT - surface->h;
	SDL_BlitSurface(surface, NULL, screen, &rect);
	SDL_FreeSurface(surface);

	// Show score
	sprintf_s(str, 40, "%02d-%02d", player1->score, player2->score);
	surface = TTF_RenderText_Solid(font52, str, fontColor);
	rect.x = (WINDOW_WIDTH - surface->w) / 2;
	rect.y = WINDOW_HEIGHT - surface->h + 2;
	SDL_BlitSurface(surface, NULL, screen, &rect);
	SDL_FreeSurface(surface);

	// Show bomb ammount
	rect_s.x = 16;
	rect_s.y = 0;
	rect_s.w = 16;
	rect_s.h = 16;
	rect.x = 150;
	rect.y = 460;
	SDL_BlitSurface(surface_bombs, &rect_s, screen, &rect);
	rect.x = 474;
	rect.y = 460;
	SDL_BlitSurface(surface_bombs, &rect_s, screen, &rect);

	sprintf_s(str, 2, "%01d", player1->bombs);
	surface = TTF_RenderText_Solid(font26, str, fontColor);
	rect.x = 180 - surface->w;
	rect.y = 460;
	SDL_BlitSurface(surface, NULL, screen, &rect);
	SDL_FreeSurface(surface);

	sprintf_s(str, 2, "%01d", player2->bombs);
	surface = TTF_RenderText_Solid(font26, str, fontColor);
	rect.x = 460;
	rect.y = 460;
	SDL_BlitSurface(surface, NULL, screen, &rect);
	SDL_FreeSurface(surface);

	// Show player avatars
	rect.x = 220 - PLAYER_W;
	rect.y = 450;
	SDL_BlitSurface(player1->sprites, player1->clip[SPR_R], screen, &rect);

	rect.x = 420;
	rect.y = 450;
	SDL_BlitSurface(player2->sprites, player2->clip[SPR_L], screen, &rect);
}

void Battle::draw_pause_screen(SDL_Surface * screen) {
	SDL_Surface * surface;
	SDL_Rect rect;
	Uint32 color;
	
	if(pause_player == player1)
		color = 0xff0000;
	else
		color = 0x0000ff;

	rect.x = (screen->w / 2) - 82;
	rect.y = (screen->h / 2) - 32;
	rect.w = 164;
	rect.h = 84;

	SDL_FillRect(screen, &rect, color);

	rect.x += 2;
	rect.y += 2;
	rect.w -= 4;
	rect.h -= 4;

	SDL_FillRect(screen, &rect, 0);

	surface = TTF_RenderText_Solid(font26, "PAUSE", fontColor);
	rect.x = (screen->w - surface->w) / 2;
	rect.y = (screen->h / 2) - surface->h - 5;

	SDL_BlitSurface(surface, NULL, screen, &rect);

	SDL_FreeSurface(surface);

	rect.x = (screen->w / 2) - 76;
	rect.y = (screen->h / 2) + 3;
	rect.w = 152;
	rect.h = 20;
	if(!pause_quit)
		rect.y = (screen->h / 2) + 23;
	SDL_FillRect(screen, &rect, color);

	surface = TTF_RenderText_Solid(font26, "RESUME", fontColor);
	rect.x = (screen->w - surface->w) / 2 ;
	rect.y = (screen->h / 2) + 25;
	SDL_BlitSurface(surface, NULL, screen, &rect);
	SDL_FreeSurface(surface);

	surface = TTF_RenderText_Solid(font26, "QUIT", fontColor);
	rect.x = (screen->w - surface->w) / 2 ;
	rect.y = (screen->h / 2) + 5;
	SDL_BlitSurface(surface, NULL, screen, &rect);
	SDL_FreeSurface(surface);
}

void Battle::draw_win_screen(SDL_Surface * screen) {
	SDL_Surface * surface;
	SDL_Rect rect;

	char * text;

	if(player1->hitpoints == 0) 
		text = (char*)"Player 2 wins";
	if(player2->hitpoints == 0)
		text = (char*)"Player 1 wins";

	surface = TTF_RenderText_Solid(font26, text, fontColor);
	rect.x = (screen->w - surface->w) / 2;
	rect.y = (screen->h - surface->h) / 2;

	SDL_BlitSurface(surface, NULL, screen, &rect);

	SDL_FreeSurface(surface);
}

void Battle::handle_draw_countdown(SDL_Surface * screen) {
	char text[5];
	SDL_Surface * surf;

	if(countdown_timer->get_ticks() >= 1000) {
		if(countdown_sec_left == 1) {
			countdown = false;
			countdown_timer->stop();

			delete countdown_timer;

			Main::audio->play(SND_GO);

			Main::audio->play_music(MUSIC_BATTLE);

			return;
		}
		countdown_sec_left--;
		countdown_timer->start();

		Main::audio->play(SND_COUNTDOWN);
	}

	if(countdown_sec_left == 4) return;

	sprintf_s(text, 5, "%d", countdown_sec_left);

	surf = TTF_RenderText_Solid(font52, text, fontColor);

	SDL_Rect rect;
	rect.x = (screen->w - surf->w) / 2;
	rect.y = (screen->h - surf->h) / 2;
	
	SDL_BlitSurface(surf, NULL, screen, &rect);
	SDL_FreeSurface(surf);
}

void Battle::load_level(const char * filename) {
	std::ifstream ifs;
	char name[20], author[20], tiles_file[30], bg_file[30];
	char tiles_file_full[35], bg_file_full[35];
	char l[1];
	SDL_Surface * surface;

	ifs.open(filename, std::ifstream::binary);
	
	ifs.read(name, 20);
	ifs.read(author, 20);
	ifs.read(tiles_file, 30);
	ifs.read(bg_file, 30);

	for(int i = 0; i < SPR_COUNT; i++) {
		ifs.read(l, 1);
		level[i] = (int)l[0];
	}

	ifs.close();

	strncpy(tiles_file_full, "gfx/\0", 5);
	strncat(tiles_file_full, tiles_file, 30);

	surface = SDL_LoadBMP(tiles_file_full);
	tiles = SDL_DisplayFormat(surface);
	SDL_FreeSurface(surface);

	if(bg_file[0] != 0) {
		strncpy(bg_file_full, "gfx/\0", 5);
		strncat(bg_file_full, bg_file, 30);

		surface = SDL_LoadBMP(bg_file_full);
		background = SDL_DisplayFormat(surface);
		SDL_FreeSurface(surface);
	} else {
		background = NULL;
	}
}

SDL_Surface * Battle::create_level_thumbnail(const char * filename) {
	std::ifstream ifs;
	char name[20], author[20], tiles_file[30], bg_file[30];
	char l[1];
	SDL_Surface * surface;
	SDL_Rect rect;
	Uint32 fillColor;
	int maxx;

	ifs.open(filename, std::ifstream::binary);
	
	ifs.read(name, 20);
	ifs.read(author, 20);
	ifs.read(tiles_file, 30);
	ifs.read(bg_file, 30);

	surface = SDL_CreateRGBSurface(NULL, SPR_COLS * 2 + 4, SPR_ROWS * 2 + 4, 32, 0, 0, 0, 0);
	SDL_FillRect(surface, NULL, 0x888888);
	rect.x = 2;
	rect.y = 2;
	rect.w = SPR_COLS * 2;
	rect.h = SPR_ROWS * 2;
	SDL_FillRect(surface, &rect, 0);
	rect.x = 2;
	rect.y = 2;
	rect.w = 2;
	rect.h = 2;
	fillColor = 0xff0000;
	maxx = SPR_COLS * 2 + 2;

	for(int i = 0; i < SPR_COUNT; i++) {
		ifs.read(l, 1);
		if(l[0] != -1) {
			SDL_FillRect(surface, &rect, fillColor);
		}
		rect.x += 2;
		if(rect.x >= maxx) {
			rect.x = 2;
			rect.y += 2;
		}
	}

	ifs.close();

	return surface;
}

void Battle::load_images() {
	SDL_Surface * surface;
	Uint32 colorkey;
	
	//surface = SDL_LoadBMP("gfx/bg.bmp");
	//background = SDL_DisplayFormat(surface);
	//SDL_FreeSurface(surface);

	//surface = SDL_LoadBMP("gfx/tiles.bmp");
	//tiles = SDL_DisplayFormat(surface);
	//SDL_FreeSurface(surface);

	surface = SDL_LoadBMP("gfx/weapons.bmp");
	weapons = SDL_DisplayFormat(surface);
	colorkey = SDL_MapRGB(weapons->format, 255, 255, 255);
	SDL_SetColorKey(weapons, SDL_SRCCOLORKEY, colorkey); 
	SDL_FreeSurface(surface);

	surface = SDL_LoadBMP("gfx/bomb.bmp");
	surface_bombs = SDL_DisplayFormat(surface);
	colorkey = SDL_MapRGB(surface_bombs->format, 0, 255, 255);
	SDL_SetColorKey(surface_bombs, SDL_SRCCOLORKEY, colorkey); 
	SDL_FreeSurface(surface);

	surface = SDL_LoadBMP("gfx/powerups.bmp");
	powerup = SDL_DisplayFormat(surface);
	colorkey = SDL_MapRGB(powerup->format, 255, 255, 255);
	SDL_SetColorKey(powerup, SDL_SRCCOLORKEY, colorkey); 
	SDL_FreeSurface(surface);

	surface = SDL_LoadBMP("gfx/player1hp.bmp");
	player1hp = SDL_DisplayFormat(surface);
	SDL_FreeSurface(surface);

	surface = SDL_LoadBMP("gfx/player2hp.bmp");
	player2hp = SDL_DisplayFormat(surface);
	SDL_FreeSurface(surface);

	set_clips();

	font26 = TTF_OpenFont("fonts/slick.ttf", 26);
	font52 = TTF_OpenFont("fonts/slick.ttf", 52);
	fontColor.r = 255;
	fontColor.g = 255;
	fontColor.b = 255;
}

void Battle::free_images() {
	if(background != NULL)
		SDL_FreeSurface(background);
	SDL_FreeSurface(tiles);

	SDL_FreeSurface(weapons);
	SDL_FreeSurface(surface_bombs);
	SDL_FreeSurface(powerup);

	SDL_FreeSurface(player1hp);
	SDL_FreeSurface(player2hp);

	for(int i = 0; i < 16; i++) {
		delete tile_rect[i];
	}
	//delete * tile_rect;

	TTF_CloseFont(font26);
	TTF_CloseFont(font52);
}

void Battle::set_clips() {
	int count = 16;
	int cols = 16;
	int row_width = SPR_W * cols;

	for (int i = 0; i < count; i++) {
		tile_rect[i] = new SDL_Rect();
		tile_rect[i]->w = SPR_W;
		tile_rect[i]->h = SPR_H;
		tile_rect[i]->x = (i * SPR_W) % row_width;
		tile_rect[i]->y = (int)(i / cols) * SPR_H;
	}
}
