#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"

#include <iostream>

#include "Main.h"

#include "Projectile.h"
#include "Bomb.h"
#include "Gameplay.h"
#include "Level.h"
#include "Player.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

const int Player::jump_height = 144;

#define PLAYER_SURF_COLS 10
#define PLAYER_SURF_COUNT 20

// Movement constantes
#define MAX_MOMENTUM_FALL 100
#define MAX_MOMENTUM_JUMP 40
#define MAX_MOMENTUM_HORIZ 20
#define MAX_MOMENTUM_HORIZ_DUCKJUMP 10
#define MAX_MOMENTUM_RUN 40

#define FRAME_CYCLE_DISTANCE 24

#define MOMENTUM_INTERV_HORIZ 1
#define MOMENTUM_INTERV_VERT 1

// Base speed
#define SPEED_HORIZ 2
#define SPEED_VERT 2

// Freeze time
#define PLAYER_FREEZE_FRAMES 15

Player::Player(const char * name, const int number, const char * sprite_file) {
	this->name = (char*)name;
	this->number = (int)number;

	speedclass = 1;
	weightclass = 1;
	bulletrateclass = 1;
	bombpowerclass = 1;

	momentumx = 0;
	momentumy = 0;

	current_sprite = SPR_R;
	distance_walked = 0;

	is_running = false;
	is_duck = false;
	is_duck_forced = false;
	duck_force_start = 0;

	is_jumping = false;
	is_falling = false;

	keydn_l = false;
	keydn_r = false;
	keydn_u = false;
	keydn_d = false;
	keydn_run = false;
	keydn_shoot = false;
	keydn_bomb = false;

	controls.use_keyboard = true;
	controls.kb_right = SDLK_RIGHT;
	controls.kb_left = SDLK_LEFT;
	controls.kb_jump = SDLK_UP;
	controls.kb_down = SDLK_DOWN;
	controls.kb_run = SDLK_RSHIFT;
	controls.kb_shoot = SDLK_RCTRL;
	controls.kb_bomb = SDLK_RALT;
	controls.kb_start = SDLK_ESCAPE;

	controls.use_joystick = true;
	controls.use_axis_x = true;
	controls.use_axis_up = false;
	controls.use_axis_down = false;
	controls.joystick_idx = 0;
	controls.js_jump = 0;
	controls.js_run = 0;
	controls.js_shoot = 1;
	controls.js_bomb = 3;
	controls.js_start = 2;

	is_hit = false;
	hit_start = 0;
	hit_delay = 30;
	hit_flicker_frame = 0;

	is_dead = false;
	dead_start = 0;

	is_frozen = false;
	freeze_start = 0;

	shoot_start = 0;
	shoot_delay = 10;

	bomb_start = 0;
	bomb_delay = 30;

	bullets = 10;
	bombs = 3;
	doubledamagebullets = 0;
	instantkillbullets = 0;

	hitpoints = 100;
	score = 0;

	cycle_direction = CYCLE_UP;

	position = new SDL_Rect();
	last_position = new SDL_Rect();
	load_images(sprite_file);
	set_clips();
}

Player::~Player() {
	for(int i = 0; i < PLAYER_SURF_COUNT; i++) {
		delete clip[i];
	}
	delete marker_clip;
	
	delete position;
	delete last_position;
	free_images();
}

void Player::show(SDL_Surface * screen) {
	draw(screen);
}

void Player::draw(SDL_Surface * screen) {
	SDL_Rect rect;

	rect.x = position->x;
	rect.y = position->y;

	if(is_dead && Gameplay::frame - dead_start > 120) {
		return;
	}

	// Check if player is hit and cycle between a show and a hide of the player to create
	// a flicker effect
	if(is_hit) {
		hit_flicker_frame = (hit_flicker_frame + 1) % 10;
		if(hit_flicker_frame < 5)
			return;
	}

	SDL_BlitSurface(sprites, clip[current_sprite], screen, &rect);

	// If the player is going out the side of the screen, we want it to
	// appear on the other side.
	if(position->x >= WINDOW_WIDTH - PLAYER_W) {
		rect.x = position->x - WINDOW_WIDTH;
		rect.y = position->y;
		SDL_BlitSurface(sprites, clip[current_sprite], screen, &rect);
	}
	if(position->x <= 0) {
		rect.x = position->x + WINDOW_WIDTH;
		rect.y = position->y;
		SDL_BlitSurface(sprites, clip[current_sprite], screen, &rect);
	}

	// Show marker if the player is above the screen
	if(position->y + position->h <= 0) {
		rect.x = position->x + ((PLAYER_W - marker_clip->w) / 2);
		rect.y = 0;
		SDL_BlitSurface(marker, marker_clip, screen, &rect);
	}
}

SDL_Rect * Player::get_rect() {
	SDL_Rect * rect;
	rect = new SDL_Rect();
	rect->x = position->x;
	rect->y = position->y;
	rect->w = PLAYER_W;
	rect->h = PLAYER_H;
	if(is_duck) {
		rect->y = rect->y + (PLAYER_H - PLAYER_DUCK_H);
		rect->h = PLAYER_DUCK_H;
	}
	return rect;
}

void Player::handle_input(SDL_Event * event) {
	handle_event(event);
}

void Player::handle_event(SDL_Event * event) {
	if(controls.use_keyboard) {
		if(event->type == SDL_KEYDOWN) {
			if(event->key.keysym.sym == controls.kb_right) {
				keydn_r = true;
			}
			if(event->key.keysym.sym == controls.kb_left) {
				keydn_l = true;
			}
			if(event->key.keysym.sym == controls.kb_jump) {
				keydn_u = true;
			}
			if(event->key.keysym.sym == controls.kb_down) {
				keydn_d = true;
			}
			if(event->key.keysym.sym == controls.kb_run) {
				keydn_run = true;
			}
			if(event->key.keysym.sym == controls.kb_shoot) {
				keydn_shoot = true;
			}
			if(event->key.keysym.sym == controls.kb_bomb) {
				keydn_bomb = true;
			}
		}
		if(event->type == SDL_KEYUP) {
			if(event->key.keysym.sym == controls.kb_right) {
				keydn_r = false;
			}
			if(event->key.keysym.sym == controls.kb_left) {
				keydn_l = false;
			}
			if(event->key.keysym.sym == controls.kb_jump) {
				keydn_u = false;
			}
			if(event->key.keysym.sym == controls.kb_down) {
				keydn_d = false;
			}
			if(event->key.keysym.sym == controls.kb_run) {
				keydn_run = false;
			}
			if(event->key.keysym.sym == controls.kb_shoot) {
				keydn_shoot = false;
			}
			if(event->key.keysym.sym == controls.kb_bomb) {
				keydn_bomb = false;
			}
		}
	}
	if(controls.use_joystick) {
		if(event->jaxis.which == controls.joystick_idx) {
			if(event->type == SDL_JOYAXISMOTION) {
				if(controls.use_axis_x && event->jaxis.axis == 0) {
					if(event->jaxis.value < -Main::JOYSTICK_AXIS_THRESHOLD) {
						keydn_l = true;
						keydn_r = false;
					}
					else if(event->jaxis.value > Main::JOYSTICK_AXIS_THRESHOLD) {
						keydn_l = false;
						keydn_r = true;
					}
					else {
						keydn_l = false;
						keydn_r = false;
					}
				}
				if(event->jaxis.axis == 1) {
					if(controls.use_axis_down && event->jaxis.value > Main::JOYSTICK_AXIS_THRESHOLD) {
						keydn_d = true;
						if(controls.use_axis_up) keydn_u = false;
					}
					else if(controls.use_axis_up && event->jaxis.value < -Main::JOYSTICK_AXIS_THRESHOLD) {
						keydn_u = true;
						if(controls.use_axis_down) keydn_d = false;
					}
					else {
						if(controls.use_axis_up) keydn_u = false;
						if(controls.use_axis_down) keydn_d = false;
					}
				}
			}
			if(event->type == SDL_JOYBUTTONDOWN) {
				if(event->jbutton.which == controls.joystick_idx) {
					if(!controls.use_axis_up && event->jbutton.button == controls.js_jump) {
						keydn_u = true;
					}
					if(!controls.use_axis_down && event->jbutton.button == controls.js_down) {
						keydn_d = true;
					}
					if(!controls.use_axis_x && event->jbutton.button == controls.js_left) {
						keydn_l = true;
					}
					if(!controls.use_axis_x && event->jbutton.button == controls.js_right) {
						keydn_r = true;
					}
					if(event->jbutton.button == controls.js_run) {
						keydn_run = true;
					}
					if(event->jbutton.button == controls.js_shoot) {
						keydn_shoot = true;
					}
					if(event->jbutton.button == controls.js_bomb) {
						keydn_bomb = true;
					}
				}
			}
			if(event->type == SDL_JOYBUTTONUP) {
				if(event->jbutton.which == controls.joystick_idx) {
					if(!controls.use_axis_up && event->jbutton.button == controls.js_jump) {
						keydn_u = false;
					}
					if(!controls.use_axis_down && event->jbutton.button == controls.js_down) {
						keydn_d = false;
					}
					if(!controls.use_axis_x && event->jbutton.button == controls.js_left) {
						keydn_l = false;
					}
					if(!controls.use_axis_x && event->jbutton.button == controls.js_right) {
						keydn_r = false;
					}
					if(event->jbutton.button == controls.js_run) {
						keydn_run = false;
					}
					if(event->jbutton.button == controls.js_shoot) {
						keydn_shoot = false;
					}
					if(event->jbutton.button == controls.js_bomb) {
						keydn_bomb = false;
					}
				}
			}
		}
	}
}

void Player::move(Level * level) {
	int speedx, speedy;
	int maxx;
	int momentumx_old;
	SDL_Rect rect;

	if(is_dead)
		return;

	last_position->x = position->x;
	last_position->y = position->y;
	last_position->w = position->w;
	last_position->h = position->h;
	
	speedx = 0;
	speedy = 0;
	
	if(is_hit) {
		// The player has been hit long enough
		if(Gameplay::frame > hit_start + hit_delay) {
			is_hit = false;
		}
	}

	if(is_frozen) {
		if(Gameplay::frame > freeze_start + PLAYER_FREEZE_FRAMES) {
			is_frozen = false;
		}
	}

	speedx = SPEED_HORIZ;

	momentumx_old = momentumx;
	
	// Are we running?
	if(keydn_run) {
		is_running = true;
		maxx = MAX_MOMENTUM_RUN;
		//maxx = speedclasses[speedclass].run_speed;
	} else {
		is_running = false;
		maxx = MAX_MOMENTUM_HORIZ;
	}
	
	// Are we ducking?
	if(keydn_d) {
		is_duck = true;
	}
	else {
		is_duck = false;
	}
	
	// Are we forced to being ducked?
	if(is_duck_forced) {
		if(Gameplay::frame - duck_force_start > DUCK_FORCE_FRAMES) {
			is_duck_forced = false;
		} else {
			is_duck = true;
		}
	}

	// Force the player to duck when bumping the head
	if(!is_duck && level->is_intersecting(position)) {
		is_duck = true;
	}

	if(is_duck) {
		if(is_jumping || is_falling) {
			// The player can move when jumping and ducking at the same time
			maxx = MAX_MOMENTUM_HORIZ_DUCKJUMP;
		} else {
			// The player cannot move when ducking and standing on the ground
			maxx = 0;
		}
	}

	if(!is_frozen && keydn_l) {
		// Move more to the left
		if(momentumx > 0) momentumx -= MOMENTUM_INTERV_HORIZ;
		if(momentumx >= -maxx) momentumx -= MOMENTUM_INTERV_HORIZ;
		else momentumx += MOMENTUM_INTERV_HORIZ;
	}
	if(!is_frozen && keydn_r) {
		// Move more to the right
		if(momentumx < 0) momentumx += MOMENTUM_INTERV_HORIZ;
		if(momentumx <= maxx) momentumx += MOMENTUM_INTERV_HORIZ;
		else momentumx -= MOMENTUM_INTERV_HORIZ;
	}
	if(is_frozen || (!keydn_l && !keydn_r)) {
		// Slide until we're standing still
		if(momentumx < 0) momentumx += MOMENTUM_INTERV_HORIZ;
		if(momentumx > 0) momentumx -= MOMENTUM_INTERV_HORIZ;
	}

	// Move the player horizontally
	speedx = (int)((double)speedx * ((double)momentumx / 10));
	position->x += speedx;
	last_speedx = speedx;

	// Which sprite do we want to show?
	if(momentumx == 0) {
		// Standing still
		if(!keydn_l && !keydn_r) {
			if(current_sprite >= SPR_L && current_sprite <= SPR_L_DUCK) {
				if(is_duck) {
					set_sprite(SPR_L_DUCK);
				} else if(is_jumping || is_falling) {
					set_sprite(SPR_L_JUMP);
				} else {
					set_sprite(SPR_L);
				}
			}
			if(current_sprite >= SPR_R && current_sprite <= SPR_R_DUCK) {
				if(is_duck) {
					set_sprite(SPR_R_DUCK);
				} else if(is_jumping || is_falling) {
					set_sprite(SPR_R_JUMP);
				} else {
					set_sprite(SPR_R);
				}
			}
		}
		else {
			if(keydn_l && !keydn_r) {
				if(is_duck)
					set_sprite(SPR_L_DUCK);
				else
					set_sprite(SPR_L_WALK1);
			}
			if(!keydn_l && keydn_r) {
				if(is_duck)
					set_sprite(SPR_R_DUCK);
				else
					set_sprite(SPR_R_WALK1);
			}
		}
		distance_walked = 0;
	}
	if(momentumx < 0) {
		// Moving left
		if(is_duck) {
			set_sprite(SPR_L_DUCK);
		} else if((is_jumping || is_falling) && !is_duck) {
			set_sprite(SPR_L_JUMP);
		} else {
			if(is_running) {
				if(current_sprite < SPR_L_RUN1 || current_sprite > SPR_L_RUN3) {
					set_sprite(SPR_L_RUN1);
				}
			} else {
				if(current_sprite < SPR_L_WALK1 || current_sprite > SPR_L_WALK3) {
					set_sprite(SPR_L_WALK1);
				}
			}
			if(keydn_r) {
				set_sprite(SPR_L_BRAKE);
				distance_walked = 0;
			}
			if(distance_walked < -FRAME_CYCLE_DISTANCE) {
				if(is_running)
					cycle_sprite(SPR_L_RUN1, SPR_L_RUN3);
				else
					cycle_sprite(SPR_L_WALK1, SPR_L_WALK3);
				distance_walked = 0;
			}
			distance_walked += speedx;
		}
	}
	else if(momentumx > 0) {
		// Moving right
		if(is_duck) {
			set_sprite(SPR_R_DUCK);
		} else if(is_jumping || is_falling) {
			set_sprite(SPR_R_JUMP);
		} else {
			if(is_running) {
				if(current_sprite < SPR_R_RUN1 || current_sprite > SPR_R_RUN3) {
					set_sprite(SPR_R_RUN1);
				}
			} else {
				if(current_sprite < SPR_R_WALK1 || current_sprite > SPR_R_WALK3) {
					set_sprite(SPR_R_WALK1);
				}
			}
			if(keydn_l) {
				set_sprite(SPR_R_BRAKE);
				distance_walked = 0;
			}
			if(distance_walked > FRAME_CYCLE_DISTANCE) {
				if(is_running)
					cycle_sprite(SPR_R_RUN1, SPR_R_RUN3);
				else
					cycle_sprite(SPR_R_WALK1, SPR_R_WALK3);
				distance_walked = 0;
			}
			distance_walked += speedx;
		}
	}

	rect.x = position->x;
	rect.y = position->y;
	rect.w = position->w;
	rect.h = position->h;
	if(is_duck) {
		// If the player is ducking, the top should be lower
		rect.y = position->y + (PLAYER_H - PLAYER_DUCK_H);
		rect.h = PLAYER_DUCK_H;
	}
	
	if(level->is_intersecting(&rect)) {
		// Stop if colliding with the level
		position->x -= speedx;
		momentumx = 0;
	}

	// Move through the sides
	// If we went too far to the right, appear at the far left (and vica versa)
	if(position->x >= WINDOW_WIDTH)
		position->x -= WINDOW_WIDTH;
	if(position->x < 0)
		position->x += WINDOW_WIDTH;
	

	// Jumping

	if(keydn_u && !is_falling && !is_jumping && !is_frozen) {
		// Start the jump
		momentumy = MAX_MOMENTUM_JUMP;
		is_jumping = true;

		Main::instance->audio->play(SND_JUMP);
	}
	if(!keydn_u && is_jumping) {
		// The up key is released, so fall faster
		is_jumping = false;
		is_falling = true;
	}
	if(is_falling || is_jumping) {
		speedy = SPEED_VERT;
		// Increase downward momentum (= decrease upward momentum)
		if(momentumy > -MAX_MOMENTUM_FALL) {
			momentumy -= MOMENTUM_INTERV_VERT;
		// Falling is faster than jumping (also.. we start to fall faster when the
		// up key is not held down)
			if(is_falling)
				momentumy  -= MOMENTUM_INTERV_VERT;
		}
	}

	speedy = (int)((double)speedy * ((double)momentumy / 10));
	
	// Move the player vertically
	position->y -= speedy;
	last_speedy = speedy;

	rect.x = position->x;
	rect.y = position->y;
	rect.w = position->w;
	rect.h = position->h;
	if(is_duck) {
		// If the player is ducking, the top should be lower
		rect.y = position->y + (PLAYER_H - PLAYER_DUCK_H);
		rect.h = PLAYER_DUCK_H;
	}

	// Did we hit something?
	if(level->is_intersecting(&rect)) {
		if(speedy > 0) {
			level->bounce_tile(&rect);
		}

		// Put the player back into the previous position
		position->y += speedy;

		if(speedy > 0) {
			// Bounce off the top (bump head)
			is_jumping = false;
			is_falling = true;
			momentumy = 0;
		} else {
			// Stop at the bottom
			is_jumping = false;
			is_falling = false;
			momentumy = 0;
		}
	}

	if(!is_jumping && !is_falling && !level->is_on_bottom(position)) {
		// start falling when there is no bottom
		is_falling = true;
	}
}

void Player::bounce(SDL_Rect * source) {
	SDL_Rect * rect;
	rect = get_rect();

	int l, r, t, b;
	int ls, rs, ts, bs;
	int diffx, diffy;
	bool is_above, is_below;
	bool is_left, is_right;

	// Collision
	l = rect->x;
	t = rect->y;
	r = rect->x + rect->w;
	b = rect->y + rect->h;

	ls = source->x;
	ts = source->y;
	rs = source->x + source->w;
	bs = source->y + source->h;

	// Bounce back
	is_above = false;
	is_below = false;
	if(t < ts) { // above the source
		diffy = b - ts;
		is_above = true;
	} else { // below the source
		diffy = bs - t;
		is_below = true;
	}

	is_left = false;
	is_right = false;
	if(l < ls) { // At the left of the source
		diffx = r - ls;
		if(diffx < 0) {
			diffx += WINDOW_WIDTH;
			is_right = true;
		} else {
			is_left = true;
		}
	} else { // At the right of the source
		diffx = rs - l;
		if(diffx < 0) {
			diffx += WINDOW_WIDTH;
			is_left = true;
		} else {
			is_right = true;
		}
	}

	if(diffx > diffy || (diffx <= 6 && diffy <= 6)) { // Players hit each others top
		if(is_above) {
			if(keydn_u) {
				Main::audio->play(SND_JUMP);
				momentumy = MAX_MOMENTUM_JUMP;
				is_falling = false;
				is_jumping = true;
			} else {
				momentumy = 30;
			}
		}
		if(is_below) {
			is_duck_forced = true;
			duck_force_start = Gameplay::frame;
			momentumy = -10;
			//hitpoints -= weightclasses[upper->weightclass].headjump_damage;
			hitpoints -= 10;
		}
	}
	if(diffx < diffy || (diffx <= 6 && diffy <= 6)) { // Players hit each others side
		if(is_left) {
			momentumx = -MAX_MOMENTUM_HORIZ;
		}
		if(is_right) {
			momentumx = MAX_MOMENTUM_HORIZ;
		}

		//momentumx = -momentumx;
		/*
		if(momxr < -(MAX_MOMENTUM_HORIZ + MOMENTUM_INTERV_HORIZ))
			left->momentumx = -weightclasses[right->weightclass].push_force_fullspeed;
			//left->momentumx = -40;
		else
			left->momentumx = -weightclasses[right->weightclass].push_force;
			//left->momentumx = -20;
		if(momxl > (MAX_MOMENTUM_HORIZ + MOMENTUM_INTERV_HORIZ))
			right->momentumx = weightclasses[left->weightclass].push_force_fullspeed;
			//right->momentumx = 40;
		else
			right->momentumx = weightclasses[left->weightclass].push_force;
			//right->momentumx = 20;
		upper->momentumy = lower->momentumy;
		*/
	}

	delete rect;
}

void Player::bounce_up() {
	//duck_force_start = Gameplay::frame;
	//is_duck_forced = true;
	is_falling = true;
	is_frozen = true;
	freeze_start = Gameplay::frame;
	//momentumy = weightclasses[weightclass].bounce_momentum;
	momentumy = 20;
	//momentumx += (player1->position->x - player2->position->x) * 2;
	if(momentumx > MAX_MOMENTUM_HORIZ)
		momentumx = MAX_MOMENTUM_HORIZ;
	if(momentumx < -MAX_MOMENTUM_HORIZ)
		momentumx = -MAX_MOMENTUM_HORIZ;
}

void Player::process() {
	if(is_dead)
		return;

	if(keydn_shoot) {
		if(Gameplay::frame > shoot_start + shoot_delay) { // &&
//			(((ruleset.bullets == BULLETS_UNLIMITED) ||  p->bullets > 0) ||
//			((ruleset.doubledamagebullets == BULLETS_UNLIMITED) ||  p->doubledamagebullets > 0) ||
//			((ruleset.instantkillbullets == BULLETS_UNLIMITED) ||  p->instantkillbullets > 0))) {
			shoot_start = Gameplay::frame;
			Projectile * pr;
			SDL_Rect * clip_weapon;
			bool doubledamage;
			bool instantkill;

//			doubledamage = ((ruleset.doubledamagebullets == BULLETS_UNLIMITED) || p->doubledamagebullets > 0);
//			instantkill = ((ruleset.instantkillbullets == BULLETS_UNLIMITED) || p->instantkillbullets > 0);

			doubledamage = false;
			instantkill = false;

			clip_weapon = new SDL_Rect();
			if(instantkill)
				clip_weapon->x = 16;
			else if(doubledamage)
				clip_weapon->x = 8;
			else
				clip_weapon->x = 0;
			clip_weapon->y = 0;
			clip_weapon->w = 8;
			clip_weapon->h = 8;

			pr = new Projectile(Main::graphics->weapons, clip_weapon);
			if(instantkill)
				pr->damage = 100;
			else if(doubledamage)
				pr->damage = 20;
			else
				pr->damage = 10;

			if(current_sprite >= SPR_L && current_sprite <= SPR_L_DUCK) {
				pr->speedx = -10;
				pr->position->x = position->x - pr->position->w - 1;
			} else {
				pr->speedx = 10;
				pr->position->x = position->x + position->w + 1;
			}
			if(is_duck)
				pr->position->y = position->y + 28;
			else
				pr->position->y = position->y + 8;
			Gameplay::instance->add_object(pr);
			
			/*for(int i = 0; i <= 1; i++) {
				Projectile * p1;
				p1 = new Projectile(Main::graphics->weapons, clip_weapon);
				p1->damage = pr->damage;
				p1->speedx = pr->speedx;
				if(i == 0) p1->speedy = -5;
				if(i == 1) p1->speedy = 5;
				p1->position->x = pr->position->x;
				p1->position->y = pr->position->y;
				Gameplay::instance->add_object(p1);
			}*/
			
			if(instantkill)
				instantkillbullets--;
			else if(doubledamage)
				doubledamagebullets--;
//			else if(ruleset.doubledamagebullets != BULLETS_UNLIMITED)
//				bullets--;

			Main::instance->audio->play(SND_SHOOT);
		}
	}
	if(keydn_bomb) {
		if(Gameplay::frame > bomb_start + bomb_delay && (bombs > 0 || bombs == -1)) {
			bomb_start = Gameplay::frame;
			Bomb * b;

			b = new Bomb(Main::graphics->bombs);
			//b->damage = bombpowerclasses[p->bombpowerclass].damage;
			b->damage = 20;
			b->time = 60;
			b->frame_start = Gameplay::frame;
			b->frame_change_start = Gameplay::frame;
			b->frame_change_count = 12;
			b->owner = this;
			b->position->x = position->x + (position->w - b->position->w) / 2;
			b->position->y = position->y + (position->h - b->position->h);
			Gameplay::instance->add_object(b);
			
			if(bombs != -1)
				bombs -= 1;

			Main::instance->audio->play(SND_SHOOT);
		}
	}
}

void Player::load_images(const char * sprite_file) {
	SDL_Surface * loaded;
	SDL_Rect rect;
	Uint32 colorkey;

	loaded = SDL_LoadBMP(sprite_file);
	if(loaded == NULL) {

	}
	sprites = SDL_DisplayFormat(loaded);
	SDL_FreeSurface(loaded);

	colorkey = SDL_MapRGB(sprites->format, 0, 255, 255); 
	SDL_SetColorKey(sprites, SDL_SRCCOLORKEY, colorkey);

	loaded = SDL_LoadBMP("gfx/pmarkers.bmp");
	marker = SDL_DisplayFormat(loaded);
	SDL_FreeSurface(loaded);

	colorkey = SDL_MapRGB(marker->format, 0, 255, 255);
	SDL_SetColorKey(marker, SDL_SRCCOLORKEY, colorkey);

	rect.w = PLAYER_W;
	rect.h = PLAYER_H;

	rect.x = 0;
	rect.y = 0;
	
	position->w = PLAYER_W;
	position->h = PLAYER_H;
}

void Player::set_clips() {
	int row_width = PLAYER_W * PLAYER_SURF_COLS;
	for (int i = 0; i < PLAYER_SURF_COUNT; i++) {
		clip[i] = new SDL_Rect();
		clip[i]->w = PLAYER_W;
		clip[i]->h = PLAYER_H;
		clip[i]->x = (i * PLAYER_W) % row_width;
		clip[i]->y = (int)(i / PLAYER_SURF_COLS) * PLAYER_H;
	}

	marker_clip = new SDL_Rect();
	marker_clip->x = 16 * (number - 1);
	marker_clip->y = 0;
	marker_clip->w = 16;
	marker_clip->h = 20;
}

void Player::free_images() {
	SDL_FreeSurface(sprites);
	SDL_FreeSurface(marker);
}

void Player::set_sprite(int sprite) {
	current_sprite = sprite;
}

void Player::cycle_sprite(int first, int last) {
	if(current_sprite < first || current_sprite > last) {
		current_sprite = first;
		return;
	}
	if(current_sprite == last)
		current_sprite = first;
	else
		current_sprite++;
}

void Player::cycle_sprite_updown(int first, int last) {
	if(current_sprite < first || current_sprite > last) {
		cycle_direction = CYCLE_UP;
		current_sprite = first;
		return;
	}
	
	if(cycle_direction == CYCLE_UP && current_sprite == last) {
			cycle_direction = CYCLE_DN;
	}
	if(cycle_direction == CYCLE_DN && current_sprite == first) {
			cycle_direction = CYCLE_UP;
	}

	if(cycle_direction == CYCLE_UP) current_sprite++;
	if(cycle_direction == CYCLE_DN) current_sprite--;
}
