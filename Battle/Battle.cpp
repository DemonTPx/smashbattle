#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"

#include <vector>
#include "crtdbg.h"

#include "Timer.h"
#include "AudioController.h"
#include "Main.h"

#include "Player.h"
#include "Projectile.h"

#include "Battle.h"

#define SPR_W 32
#define SPR_H 32
#define SPR_COLS 20
#define SPR_ROWS 15
#define SPR_COUNT (20 * 15)

#define MAX_MOMENTUM_FALL 100
#define MAX_MOMENTUM_JUMP 40
#define MAX_MOMENTUM_HORIZ 20
#define MAX_MOMENTUM_RUN 40

#define FRAME_CYCLE_DISTANCE 24

#define MOMENTUM_INTERV_HORIZ 1
#define MOMENTUM_INTERV_VERT 1

#define SPEED_HORIZ 2
#define SPEED_VERT 2

const int Battle::level[SPR_COUNT] = 
{ -1,-1,-1,-1,-1, -1,-1,-1,-1,-1, -1,-1,-1,-1,-1, -1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1, -1,-1,-1,-1,-1, -1,-1,-1,-1,-1, -1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1, -1,-1,-1,-1,-1, -1,-1,-1,-1,-1, -1,-1,-1,-1,-1,
   0, 0, 0, 0, 0,  0, 0,-1,-1,-1, -1,-1,-1, 0, 0,  0, 0, 0, 0, 0,
  -1,-1,-1,-1,-1, -1,-1,-1,-1,-1, -1,-1,-1,-1,-1, -1,-1,-1,-1,-1,

  -1,-1,-1,-1,-1, -1,-1,-1,-1,-1, -1,-1,-1,-1,-1, -1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1, -1, 0, 0, 0, 0,  0, 0, 0, 0,-1, -1,-1,-1,-1,-1,
   0, 0, 0,-1,-1, -1,-1,-1,-1,-1, -1,-1,-1,-1,-1, -1,-1, 0, 0, 0,
  -1,-1,-1,-1,-1, -1,-1,-1,-1,-1, -1,-1,-1,-1,-1, -1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1, -1,-1,-1,-1,-1, -1,-1,-1,-1,-1, -1,-1,-1,-1,-1,
  
   0, 0, 0, 0, 0,  0, 0,-1,-1,-1, -1,-1,-1, 0, 0,  0, 0, 0, 0, 0,
  -1,-1,-1,-1,-1, -1,-1,-1,-1,-1, -1,-1,-1,-1,-1, -1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1, -1,-1,-1,-1,-1, -1,-1,-1,-1,-1, -1,-1,-1,-1,-1,
   1, 1, 1, 1, 1,  1, 1, 1, 1, 1,  1, 1, 1, 1, 1,  1, 1, 1, 1, 1,
  -1,-1,-1,-1,-1, -1,-1,-1,-1,-1, -1,-1,-1,-1,-1, -1,-1,-1,-1,-1 };
  

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

	player1 = new Player("gfx/bert.bmp");
	player1->key_l = SDLK_a;
	player1->key_r = SDLK_d;
	player1->key_u = SDLK_w;
	player1->key_run = SDLK_LSHIFT;
	player1->key_shoot = SDLK_LCTRL;
	player1->joystick_idx = 0;
	player1->js_btn_u = 1;
	player1->js_btn_shoot = 2;
	player1->js_btn_start = 9;

	player2 = new Player("gfx/jeroen.bmp");
	player2->key_l = SDLK_LEFT;
	player2->key_r = SDLK_RIGHT;
	player2->key_u = SDLK_UP;
	player2->key_run = SDLK_RSHIFT;
	player2->key_shoot = SDLK_RCTRL;
	player2->joystick_idx = 1;
	player2->js_btn_u = 0;
	player2->js_btn_shoot = 1;
	player2->js_btn_start = 7;

	projectiles = new std::vector<Projectile*>(0);

	reset_game();

	while (Main::running && game_running) {
		// Event handling
		while(SDL_PollEvent(&event)) {
			Main::instance->handle_event(&event);
			if(event.type == SDL_KEYDOWN) {
				if(event.key.keysym.sym == SDLK_ESCAPE) {
					paused = !paused;

					if(paused) {
						Main::instance->audio->pause_music();
						if(countdown) countdown_timer->pause();
					} else {
						Main::instance->audio->unpause_music();
						if(countdown) countdown_timer->unpause();
					}
					Main::instance->audio->play_pause();
				}
				if(event.key.keysym.sym == SDLK_n) {
					if(paused) {
						paused = false;
						if(countdown) countdown_timer->unpause();
						Main::instance->audio->unpause_music();
						Main::instance->audio->play_pause();
					}
				}
				if(event.key.keysym.sym == SDLK_y) {
					if(paused) game_running = false;
				}
			}
			if(event.type == SDL_JOYBUTTONDOWN) {
				if((event.jbutton.which == player1->joystick_idx && event.jbutton.button == player1->js_btn_start) ||
					event.jbutton.which == player2->joystick_idx && event.jbutton.button == player2->js_btn_start) {
					paused = !paused;

					if(paused) {
						Main::instance->audio->pause_music();
						if(countdown) countdown_timer->pause();
					} else {
						Main::instance->audio->unpause_music();
						if(countdown) countdown_timer->unpause();
					}
					Main::instance->audio->play_pause();
				}
			}
			player1->handle_input(&event);
			player2->handle_input(&event);
		}

		// Processing
		if(!paused && !countdown && !ended) {

			move_player(player1);
			move_player(player2);

			process_shoot(player1);
			process_shoot(player2);

			check_player_collision(player1, player2);

			check_player_projectile_collision(player1);
			check_player_projectile_collision(player2);

			for(unsigned int idx = 0; idx < projectiles->size(); idx++) {
				Projectile * p = projectiles->at(idx);
				move_projectile(p);
				if(p->hit) {
					projectiles->erase(projectiles->begin() + idx);
				}
			}
			
			if(player1->hitpoints <= 0) {
				player2->score++;
				player1->is_hit = true;
				player2->is_hit = false;
				ended = true;
				end_timer = new Timer();
				end_timer->start();
			}
			if(player2->hitpoints <= 0) {
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
	
	projectiles->clear();
	delete projectiles;

	free_images();
}

void Battle::reset_game() {
	player1->position->x = 160;
	player1->position->y = 320 - player1->position->h;
	player1->is_hit = false;
	player1->hitpoints = 100;
	player1->shoot_start = 0;
	player1->hit_start = 0;
	player1->momentumx = 0;
	player1->momentumy = 0;
	player1->set_sprite(SPR_R);

	player2->position->x = 448;
	player2->position->y = 320 - player2->position->h;
	player2->is_hit = false;
	player2->hitpoints = 100;
	player2->shoot_start = 0;
	player2->hit_start = 0;
	player1->momentumx = 0;
	player1->momentumy = 0;
	player2->set_sprite(SPR_L);

	projectiles->clear();

	frame = 0;
	paused = false;
	ended = false;
	countdown = true;
	countdown_sec_left = 4;
	countdown_timer = new Timer();
	countdown_timer->start();

	Main::audio->stop_music();
}

void Battle::process_shoot(Player * p) {
	if(p->keydn_shoot) {
		if(frame > p->shoot_start + p->shoot_delay) {
			p->shoot_start = frame;
			Projectile * pr;
			SDL_Rect * clip_weapon;

			clip_weapon = new SDL_Rect();
			clip_weapon->x = 0;
			clip_weapon->y = 0;
			clip_weapon->w = 6;
			clip_weapon->h = 6;

			pr = new Projectile("gfx/weapons.bmp", clip_weapon);

			if(p->current_sprite >= SPR_L && p->current_sprite <= SPR_L_WALK3 || p->current_sprite == SPR_L_JUMP || p->current_sprite == SPR_R_BRAKE) {
				pr->speedx = -10;
				pr->position->x = p->position->x - pr->position->w - 1;
			} else {
				pr->speedx = 10;
				pr->position->x = p->position->x + p->position->w + 1;
			}
			pr->position->y = p->position->y + 12;
			projectiles->push_back(pr);

			Main::instance->audio->play_shoot();
		}
	}
}

void Battle::move_player(Player * p) {
	int speedx, speedy;
	int maxx;
	int momentumx_old;
	
	speedx = 0;
	speedy = 0;

	if(p->is_hit) {
		if(frame > p->hit_start + p->hit_delay) {
			p->is_hit = false;
		}
	}

	speedx = SPEED_HORIZ;

	momentumx_old = p->momentumx;

	if(p->keydn_run) {
		p->is_running = true;
		maxx = MAX_MOMENTUM_RUN;
	} else {
		p->is_running = false;
		maxx = MAX_MOMENTUM_HORIZ;
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
		if(!p->keydn_l && !p->keydn_r) {
			if((p->current_sprite >= SPR_L && p->current_sprite <= SPR_L_WALK3) || p->current_sprite == SPR_R_BRAKE || p->current_sprite == SPR_L_JUMP) {
				if(p->is_jumping || p->is_falling) {
					p->set_sprite(SPR_L_JUMP);
				} else {
					p->set_sprite(SPR_L);
				}
			}
			if((p->current_sprite >= SPR_R && p->current_sprite <= SPR_R_WALK3) || p->current_sprite == SPR_L_BRAKE || p->current_sprite == SPR_R_JUMP) {
				if(p->is_jumping || p->is_falling) {
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
				p->set_sprite(SPR_L_WALK1);
			}
			if(!p->keydn_l && p->keydn_r) {
				p->set_sprite(SPR_R_WALK1);
			}
		}
		p->distance_walked = 0;
	}
	if(p->momentumx < 0) {
		if(p->is_jumping || p->is_falling) {
			p->set_sprite(SPR_L_JUMP);
		}
		else {
			if(p->current_sprite < SPR_L_WALK1 || p->current_sprite > SPR_L_WALK3) {
				p->set_sprite(SPR_L_WALK1);
			}
			if(p->keydn_r) {
				p->set_sprite(SPR_L_BRAKE);
				p->distance_walked = 0;
			}
			if(p->distance_walked < -FRAME_CYCLE_DISTANCE) {
				p->cycle_sprite_updown(SPR_L_WALK1, SPR_L_WALK3);
				p->distance_walked = 0;
			}
			p->distance_walked += speedx;
		}
	}
	else if(p->momentumx > 0) {
		if(p->is_jumping || p->is_falling) {
			p->set_sprite(SPR_R_JUMP);
		}
		else {
			if(p->current_sprite < SPR_R_WALK1 || p->current_sprite > SPR_R_WALK3) {
				p->set_sprite(SPR_R_WALK1);
			}
			if(p->keydn_l) {
				p->set_sprite(SPR_R_BRAKE);
				p->distance_walked = 0;
			}
			if(p->distance_walked > FRAME_CYCLE_DISTANCE) {
				p->cycle_sprite_updown(SPR_R_WALK1, SPR_R_WALK3);
				p->distance_walked = 0;
			}
			p->distance_walked += speedx;
		}
	}
	
	if(check_collision(p->position)) {
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

		Main::instance->audio->play_jump();
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

	if(check_collision(p->position)) {
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
		// fall when there is no bottom
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

void Battle::check_player_collision(Player * p1, Player * p2) {
	// Check if there is a collision between the players and process the further movement
	int l1, r1, t1, b1;
	int l2, r2, t2, b2;
	int momxl, momyl;
	int momxr, momyr;
	int diffx, diffy;
	Player * upper, * lower;
	Player * left, * right;

	l1 = p1->position->x;
	t1 = p1->position->y;
	r1 = p1->position->x + p1->position->w;
	b1 = p1->position->y + p1->position->h;

	l2 = p2->position->x;
	t2 = p2->position->y;
	r2 = p2->position->x + p2->position->w;
	b2 = p2->position->y + p2->position->h;

	// Return if not colliding
	if(b1 <= t2) return;
	if(t1 >= b2) return;
	if(r1 <= l2) {
		if(r1 > WINDOW_WIDTH || r2 > WINDOW_WIDTH) {
			if((r1 - WINDOW_WIDTH <= l2) && (l1 >= r2 - WINDOW_WIDTH))
				return;
		} else {
			return;
		}
	}
	if(l1 >= r2) {
		if(r1 > WINDOW_WIDTH || r2 > WINDOW_WIDTH) {
			if((r1 - WINDOW_WIDTH <= l2) && (l1 >= r2 - WINDOW_WIDTH))
				return;
		} else {
			return;
		}
	}

	// Collision

	Main::audio->play_bounce();

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

	/*
	if(r1 > WINDOW_WIDTH) {
		l1 -= WINDOW_WIDTH;
		r1 -= WINDOW_WIDTH;
	}
	if(r2 > WINDOW_WIDTH) {
		l2 -= WINDOW_WIDTH;
		r2 -= WINDOW_WIDTH;
	}
	*/
	
	bool p1_clip = false;
	bool p2_clip= false;
	
	if(r1 > WINDOW_WIDTH) {
		p1_clip = true;
	}
	if(r2 > WINDOW_WIDTH) {
		p2_clip = true;
	}

	if(p1->momentumx > p2->momentumx) { // p1 at the left of p2
		diffx = r1 - l2;
		if(p1_clip) diffx -= WINDOW_WIDTH;
		left = p1;
		right = p2;

		momxl = p1->momentumx;
		momyl = p1->momentumy;

		momxr = p2->momentumx;
		momyr = p2->momentumy;
	} else { // p1 at the right of p2, or at the same level
		diffx = r2 - l1;
		left = p2;
		right = p1;

		momxl = p2->momentumx;
		momyl = p2->momentumy;

		momxr = p1->momentumx;
		momyr = p1->momentumy;
	}

	if(diffx > diffy) { // Players hit each others top
		if(upper->keydn_u) {
			Main::audio->play_jump();
			upper->momentumy = MAX_MOMENTUM_JUMP;
			upper->is_falling = false;
			upper->is_jumping = true;
		} else {
			upper->momentumy = 30;
		}
		lower->momentumy = -10;
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
	
	/*
	// Default bouncing speed
	if(p1->momentumx < 0) base1 = 5;
	else if(p1->momentumx > 0) base1 = -5;
	else base1 = 0;

	if(p2->momentumx < 0) base2 = 5;
	else if(p2->momentumx > 0) base2 = -5;
	else base2 = 0;

	p1->momentumx = -(momx1 / 2) + (momx2 / 2) + base1;
	p1->momentumy = -(momy1 / 2);

	p2->momentumx = -(momx2 / 2) + (momx1 / 2) + base2;
	p2->momentumy = -(momy2 / 2);


	if(b1 > t2 && b1 < b2) {
		// Player 1 is above player 2
		diffy = b1 - t2;
		if(l1 < l2) {
			diffx = r1 - l2;
		} else {
			diffx = r2 - l1;
		}
		if(diffy < diffx) {
			// Hit from above (hit more of the width than the height)
			p1->momentumy += 20;
		}
	}
	if(b2 > t1 && b2 < b1) {
		// Player 1 is above player 2
		diffy = b2 - t1;
		if(l2 < l1) {
			diffx = r2 - l1;
		} else {
			diffx = r1 - l2;
		}
		if(diffy < diffx) {
			// Hit from above (hit more of the width than the height)
			p2->momentumy += 20;
		}
	}
	*/
}

void Battle::check_player_projectile_collision(Player * p) {
	Projectile * pr;
	int l1, r1, t1, b1;
	int l2, r2, t2, b2;

	if(p->is_hit) return;
	
	l1 = p->position->x;
	r1 = p->position->x + p->position->w;
	t1 = p->position->y;
	b1 = p->position->y + p->position->h;

	/*
	if(l1 >= WINDOW_WIDTH) l1 = l1 - WINDOW_WIDTH;
	if(r1 >= WINDOW_WIDTH) r1 = r1 - WINDOW_WIDTH;

	if(t1 >= WINDOW_HEIGHT) t1 = t1 - WINDOW_HEIGHT;
	if(b1 >= WINDOW_HEIGHT) b1 = b1 - WINDOW_HEIGHT;

	if(t1 < 0) t1 = 0;
	if(b1 > WINDOW_HEIGHT) b1 = WINDOW_HEIGHT; */

	for(unsigned int idx = 0; idx < projectiles->size(); idx++) {
		pr = projectiles->at(idx);
		l2 = pr->position->x;
		r2 = pr->position->x + pr->position->w;
		t2 = pr->position->y;
		b2 = pr->position->y + pr->position->h;

		if(l1 > r2) continue;
		if(r1 < l2) continue;
		if(t1 > b2) continue;
		if(b1 < t2) continue;

		pr->hit = true;
		p->is_hit = true;
		p->hit_start = frame;
		p->hitpoints -= 10;

		Main::audio->play_hit();
	}
}

bool Battle::check_collision(SDL_Rect * rect) {
	// Check if the rect is colliding with the level
	int l, r, t, b;

	l = rect->x;
	r = rect->x + rect->w - 1;

	t = rect->y;
	b = rect->y + rect->h - 1;

	//if(t >= WINDOW_HEIGHT) t = t - WINDOW_HEIGHT;
	//if(b >= WINDOW_HEIGHT) b = b - WINDOW_HEIGHT;

	if(t < 0) t = 0;
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


	/*
	if(level[level_pos(l, t)] != -1)
		return true;
	if(level[level_pos(l, b)] != -1)
		return true;
	if(level[level_pos(r, t)] != -1)
		return true;
	if(level[level_pos(r, b)] != -1)
		return true;*/
	return false;
}

bool Battle::check_bottom(SDL_Rect * rect) {
	// Check if there is anything to stand on below the rect
	int l, r, t, b;

	l = rect->x;
	r = rect->x + rect->w - 1;

	t = rect->y;
	b = rect->y + rect->h - 1;

	if(l >= WINDOW_WIDTH) l -= WINDOW_WIDTH;
	if(r >= WINDOW_WIDTH) r -= WINDOW_WIDTH;

	if(level[level_pos(l, b + 1)] != -1)
		return false;
	if(level[level_pos(r, b + 1)] != -1)
		return false;
	return true;
}

int Battle::level_pos(int x, int y) {
	// Calculate in which grid position the x and y coordinates are
	return ((int)(y / SPR_H) * SPR_COLS) + (int)(x / SPR_W);
}

void Battle::draw_level(SDL_Surface * screen) {
	SDL_Rect rect;

	rect.w = SPR_W;
	rect.h = SPR_H;

	//SDL_FillRect(screen, &screen->clip_rect, 0);
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

	char str[40];

	sprintf_s(str, 40, "Player1 HP: %d", player1->hitpoints);

	surface = TTF_RenderText_Solid(font26, str, fontColor);
	rect.x = 2;
	rect.y = WINDOW_HEIGHT - surface->h - 2;

	SDL_BlitSurface(surface, NULL, screen, &rect);

	SDL_FreeSurface(surface);


	sprintf_s(str, 40, "Player2 HP: %d", player2->hitpoints);

	surface = TTF_RenderText_Solid(font26, str, fontColor);
	rect.x = WINDOW_WIDTH - surface->w - 2;
	rect.y = WINDOW_HEIGHT - surface->h - 2;

	SDL_BlitSurface(surface, NULL, screen, &rect);

	SDL_FreeSurface(surface);


	sprintf_s(str, 40, "%02d:%02d", player1->score, player2->score);

	surface = TTF_RenderText_Solid(font52, str, fontColor);
	rect.x = (WINDOW_WIDTH - surface->w) / 2;
	rect.y = WINDOW_HEIGHT - surface->h + 2;

	SDL_BlitSurface(surface, NULL, screen, &rect);

	SDL_FreeSurface(surface);
}

void Battle::draw_pause_screen(SDL_Surface * screen) {
	SDL_Surface * surface;
	SDL_Rect rect;

	surface = TTF_RenderText_Solid(font26, "QUIT GAME Y/N", fontColor);
	rect.x = (screen->w - surface->w) / 2;
	rect.y = (screen->h - surface->h) / 2;

	SDL_BlitSurface(surface, NULL, screen, &rect);

	SDL_FreeSurface(surface);
}

void Battle::draw_win_screen(SDL_Surface * screen) {
	SDL_Surface * surface;
	SDL_Rect rect;

	char * text;

	if(player1->hitpoints == 0) 
		text = "Player 2 wins";
	if(player2->hitpoints == 0)
		text = "Player 1 wins";

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

			Main::audio->play_go();

			Main::audio->play_music(MUSIC_BATTLE);

			return;
		}
		countdown_sec_left--;
		countdown_timer->start();

		Main::audio->play_countdown();
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

void Battle::load_images() {
	SDL_Surface * surface;
	
	surface = SDL_LoadBMP("gfx/bg.bmp");
	background = SDL_DisplayFormat(surface);
	SDL_FreeSurface(surface);

	surface = SDL_LoadBMP("gfx/tiles.bmp");
	tiles = SDL_DisplayFormat(surface);
	SDL_FreeSurface(surface);

	set_clips();

	font26 = TTF_OpenFont("fonts/slick.ttf", 26);
	font52 = TTF_OpenFont("fonts/slick.ttf", 52);
	fontColor.r = 255;
	fontColor.g = 255;
	fontColor.b = 255;
}

void Battle::free_images() {
	SDL_FreeSurface(tiles);
	SDL_FreeSurface(background);

	delete * tile_rect;

	TTF_CloseFont(font26);
	TTF_CloseFont(font52);
}

void Battle::set_clips() {
	int count = 8;
	int cols = 8;
	int row_width = SPR_W * cols;

	for (int i = 0; i < count; i++) {
		tile_rect[i] = new SDL_Rect();
		tile_rect[i]->w = SPR_W;
		tile_rect[i]->h = SPR_H;
		tile_rect[i]->x = (i * SPR_W) % row_width;
		tile_rect[i]->y = (int)(i / cols) * SPR_H;
	}
}
