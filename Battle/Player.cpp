#include "SDL/SDL.h"

#include <iostream>

#include "Main.h"

#include "Projectile.h"
#include "Bomb.h"
#include "Gameplay.h"
#include "Level.h"
#include "Player.h"

#define BULLETS_UNLIMITED -1

const int Player::CHARACTER_COUNT = 16;
const Character Player::CHARACTERS[Player::CHARACTER_COUNT] = {
	//      Name               Filename            sp wt wp bd
	{(char*)"BERT",		(char*)"gfx/bert.bmp",		1, 1, 1, 1},
	{(char*)"JEROEN",	(char*)"gfx/jeroen.bmp",	2, 1, 1, 0},
	{(char*)"STEVEN",	(char*)"gfx/steven.bmp",	0, 2, 1, 1},
	{(char*)"TEDJE",	(char*)"gfx/tedje.bmp",		0, 1, 2, 1},
	{(char*)"OKKE",		(char*)"gfx/okke.bmp",		2, 1, 0, 1},
	{(char*)"JEREMY",	(char*)"gfx/jeremy.bmp",	1, 0, 1, 2},
	{(char*)"MARCEL",	(char*)"gfx/marcel.bmp",	1, 0, 2, 1},
	{(char*)"JAY",		(char*)"gfx/jay.bmp",		2, 1, 1, 0},
	{(char*)"DONJA",	(char*)"gfx/donja.bmp",		2, 0, 1, 1},
	{(char*)"ROB",		(char*)"gfx/rob.bmp",		0, 2, 0, 2},
	{(char*)"EVA",		(char*)"gfx/eva.bmp",		1, 2, 0, 1},
	{(char*)"TOBIAS",	(char*)"gfx/tobias.bmp",	1, 1, 2, 0},
	{(char*)"ARJAN",	(char*)"gfx/arjan.bmp",		0, 2, 2, 0},
	{(char*)"RUUD",		(char*)"gfx/ruud.bmp",		2, 0, 0, 2},
	{(char*)"ANTON",	(char*)"gfx/anton.bmp",		1, 1, 0, 2},
	{(char*)"RUTGER",	(char*)"gfx/rutger.bmp",	2, 0, 2, 0},
};
const int Player::COLORS[4] = {
	0xaa0000,
	0x0000aa,
	0x009900,
	0xaa9900,
};

const int Player::SPEEDCLASS_COUNT = 3;
const SpeedClass Player::SPEEDCLASSES[Player::SPEEDCLASS_COUNT] = {
	{30},
	{40},
	{50},
};
const int Player::WEIGHTCLASS_COUNT = 3;
const WeightClass Player::WEIGHTCLASSES[Player::WEIGHTCLASS_COUNT] = {
	{5,   0, 35, 20},
	{10,  5, 30, 15},
	{15, 10, 25, 10},
};
const int Player::WEAPONCLASS_COUNT = 3;
const WeaponClass Player::WEAPONCLASSES[Player::WEAPONCLASS_COUNT] = {
	{12, 250, 10},
	{10, 300, 10},
	{ 8, 350, 10},
};
const int Player::BOMBPOWERCLASS_COUNT = 3;
const BombPowerClass Player::BOMBPOWERCLASSES[Player::BOMBPOWERCLASS_COUNT] = {
	{15},
	{20},
	{25},
};

const int Player::jump_height = 144;

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
#define PLAYER_FREEZE_FRAMES 30

Player::Player(int character, int number) {
	name = CHARACTERS[character].name;
	this->number = number;

	speedclass = CHARACTERS[character].speedclass;
	weightclass = CHARACTERS[character].weightclass;
	weaponclass = CHARACTERS[character].weaponclass;
	bombpowerclass = CHARACTERS[character].bombpowerclass;

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

	rounds_won = 0;

	reset();

	position = new SDL_Rect();
	last_position = new SDL_Rect();

	position->w = PLAYER_W;
	position->h = PLAYER_H;

	sprites = Main::graphics->player->at(character);
	marker_clip = Main::graphics->pmarker_clip[(number - 1)];
}

Player::~Player() {
	delete position;
	delete last_position;
}

void Player::set_character(int character) {
	name = CHARACTERS[character].name;

	speedclass = CHARACTERS[character].speedclass;
	weightclass = CHARACTERS[character].weightclass;
	weaponclass = CHARACTERS[character].weaponclass;
	bombpowerclass = CHARACTERS[character].bombpowerclass;

	sprites = Main::graphics->player->at(character);
}

void Player::reset() {
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

	is_hit = false;
	hit_start = 0;
	hit_delay = 30;
	hit_flicker_frame = 0;

	is_dead = false;
	dead_start = 0;

	is_frozen = false;
	freeze_start = 0;

	bounce_direction_x = 0;
	bounce_direction_y = 0;

	shoot_start = 0;
	shoot_delay = 10;

	bomb_start = 0;
	bomb_delay = 30;

	bullets = 10;
	bombs = 3;
	doubledamagebullets = 0;
	instantkillbullets = 0;

	score = 0;
	hitpoints = 100;

	bullets_fired = 0;
	bullets_hit = 0;
	bombs_fired = 0;
	bombs_hit = 0;
	headstomps = 0;

	cycle_direction = CYCLE_UP;
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

	SDL_BlitSurface(sprites, Main::graphics->player_clip[current_sprite], screen, &rect);

	// If the player is going out the side of the screen, we want it to
	// appear on the other side.
	if(position->x >= WINDOW_WIDTH - PLAYER_W) {
		rect.x = position->x - WINDOW_WIDTH;
		rect.y = position->y;
		SDL_BlitSurface(sprites, Main::graphics->player_clip[current_sprite], screen, &rect);
	}
	if(position->x <= 0) {
		rect.x = position->x + WINDOW_WIDTH;
		rect.y = position->y;
		SDL_BlitSurface(sprites, Main::graphics->player_clip[current_sprite], screen, &rect);
	}

	// Show marker if the player is above the screen
	if(position->y + position->h <= 0) {
		rect.x = position->x + ((PLAYER_W - marker_clip->w) / 2);
		rect.y = 0;
		SDL_BlitSurface(Main::graphics->pmarker, marker_clip, screen, &rect);
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
	if(is_duck) {
		last_position->y = last_position->y + (PLAYER_H - PLAYER_DUCK_H);
		last_position->h = PLAYER_DUCK_H;
	}
	
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
		//maxx = MAX_MOMENTUM_RUN;
		maxx = SPEEDCLASSES[speedclass].run_speed;
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

	// Die when we fall out of the level
	if(position->y + position->h > (14 * TILE_H)) {
		hitpoints = 0;
	}
}

void Player::bounce(Player * other) {
	SDL_Rect * rect, * source;

	rect = last_position;
	source = other->last_position;

	int l, r, t, b;
	int ls, rs, ts, bs;
	bool is_above, is_below;
	bool is_left, is_right;

	l = rect->x;
	t = rect->y;
	r = rect->x + rect->w;
	b = rect->y + rect->h;

	ls = source->x;
	ts = source->y;
	rs = source->x + source->w;
	bs = source->y + source->h;

	if(l - ls > (WINDOW_WIDTH / 2)) {
		ls += WINDOW_WIDTH;
		rs += WINDOW_WIDTH;
	}
	if(ls - l > (WINDOW_WIDTH / 2)) {
		l += WINDOW_WIDTH;
		r += WINDOW_WIDTH;
	}

	is_above = (b <= ts);
	is_below = (t >= bs);
	is_left = (r <= ls);
	is_right = (l >= rs);

	if(!is_above && !is_below && !is_left && !is_right) {
		if(bounce_direction_y == -1) is_above = true;
		if(bounce_direction_y == 1) is_below = true;
		if(bounce_direction_x == -1) is_left = true;
		if(bounce_direction_x == 1) is_right = true;
	}

	// Players hit each others top
	if(is_above) {
		bounce_direction_y = -1;
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
		bounce_direction_y = 1;
		is_duck_forced = true;
		duck_force_start = Gameplay::frame;
		if(!is_hit) {
			is_hit = true;
			hit_start = Gameplay::frame;
			momentumy = -10;
			hitpoints -= WEIGHTCLASSES[other->weightclass].headjump_damage;
			other->headstomps++;
		}
	}
	if(!is_above && !is_below) {
		bounce_direction_y = 0;
	}

	newmomentumx = momentumx;
	// Players hit each others side
	if(is_left || is_right) {
		newmomentumx = other->momentumx;
		
		if(is_left) {
			bounce_direction_x = -1;
			newmomentumx -= WEIGHTCLASSES[other->weightclass].push_force - WEIGHTCLASSES[weightclass].push_force + 10;
		}
		if(is_right) {
			bounce_direction_x = 1;
			newmomentumx += WEIGHTCLASSES[other->weightclass].push_force - WEIGHTCLASSES[weightclass].push_force + 10;
		}
	} else {
		bounce_direction_x = 0;
	}
}

void Player::bounce_up(SDL_Rect * source) {
	is_falling = true;
	is_frozen = true;
	freeze_start = Gameplay::frame;
	momentumy = WEIGHTCLASSES[weightclass].bounce_momentum_x;
	if(position->x < source->x) {
		momentumx -= WEIGHTCLASSES[weightclass].bounce_momentum_y;
	}
	if(position->x > source->x) {
		momentumx += WEIGHTCLASSES[weightclass].bounce_momentum_y;
	}
	if(momentumx > MAX_MOMENTUM_HORIZ)
		momentumx = MAX_MOMENTUM_HORIZ;
	if(momentumx < -MAX_MOMENTUM_HORIZ)
		momentumx = -MAX_MOMENTUM_HORIZ;
}

void Player::process() {
	if(is_dead)
		return;

	if(keydn_shoot) {
		if(Gameplay::frame > shoot_start + WEAPONCLASSES[weaponclass].rate &&
			(((bullets == BULLETS_UNLIMITED) ||  bullets > 0) ||
			((doubledamagebullets == BULLETS_UNLIMITED) ||  doubledamagebullets > 0) ||
			((instantkillbullets == BULLETS_UNLIMITED) ||  instantkillbullets > 0))) {
			shoot_start = Gameplay::frame;
			Projectile * pr;
			SDL_Rect * clip_weapon;
			bool doubledamage;
			bool instantkill;

			doubledamage = doubledamagebullets > 0;
			instantkill = instantkillbullets > 0;

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
			pr->owner = this;
			if(instantkill)
				pr->damage = 100;
			else if(doubledamage)
				pr->damage = WEAPONCLASSES[weaponclass].damage * 2;
			else
				pr->damage = WEAPONCLASSES[weaponclass].damage;

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

			pr->max_distance = WEAPONCLASSES[weaponclass].distance;
			
			if(instantkill)
				instantkillbullets--;
			else if(doubledamage)
				doubledamagebullets--;
//			else if(ruleset.doubledamagebullets != BULLETS_UNLIMITED)
//				bullets--;
			
			bullets_fired++;

			Main::instance->audio->play(SND_SHOOT);
		}
	}
	if(keydn_bomb) {
		if(Gameplay::frame > bomb_start + bomb_delay && (bombs > 0 || bombs == -1)) {
			bomb_start = Gameplay::frame;
			Bomb * b;

			b = new Bomb(Main::graphics->bombs);
			b->damage = BOMBPOWERCLASSES[bombpowerclass].damage;
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

			bombs_fired++;

			Main::instance->audio->play(SND_SHOOT);
		}
	}
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
