#include "SDL/SDL.h"

#include <iostream>

#include "Main.h"

#include "network/ServerClient.h"
#include "network/Server.h"
#include "Projectile.h"
#include "Bomb.h"
#include "Mine.h"
#include "Gameplay.h"
#include "Level.h"
#include "Player.h"
#include "util/Log.h"
#include "util/ServerUtil.h"
#include "commands/CommandShotFired.hpp"
#include "commands/CommandBombDropped.hpp"
#include "commands/CommandSetHitPoints.hpp"

#define BULLETS_UNLIMITED -1

const int Player::CHARACTER_COUNT = 20;
#define TWEAKERS
#ifdef TWEAKERS
const Character Player::CHARACTERS[Player::CHARACTER_COUNT] = {
	//      Name               	Filename            			sp wt wp bd
	{(char *)"INSPECTOR", 		(char *)"gfx/jeroen.bmp",		1, 1, 1, 1},
	{(char *)"MISHA", 		(char *)"gfx/misha.bmp",		1, 2, 1, 0},
	{(char *)"DEMONTPX",		(char *)"gfx/bert.bmp",                 1, 1, 2, 0},
	{(char *)"TRIGEN", 		(char *)"gfx/ray.bmp",			2, 1, 0, 1},
	{(char *)"CRISP", 		(char *)"gfx/tino.bmp",			2, 1, 1, 0},
	{(char *)"WILBERT", 		(char *)"gfx/wilbert.bmp",		1, 1, 0, 2},
	{(char *)"ACM", 		(char *)"gfx/arjen.bmp",		1, 0, 1, 2},
	{(char *)"FEMME", 		(char *)"gfx/femme.bmp",		0, 1, 2, 1},
	{(char *)"MAITE", 		(char *)"gfx/maite.bmp",		2, 0, 2, 0},
	{(char *)"ZEEF", 		(char *)"gfx/frederik.bmp",		0, 2, 1, 1},
	{(char *)"WOUT", 		(char *)"gfx/wout.bmp",			2, 1, 0, 1},
	{(char *)"PAUL", 		(char *)"gfx/paul.bmp",			1, 1, 2, 0},
	{(char *)"JJJUR", 		(char *)"gfx/jur.bmp",			2, 0, 1, 1},
	{(char *)"ROB_ERWT", 		(char *)"gfx/robert.bmp",		0, 2, 2, 0},
	{(char *)"ARNOUD",       	(char *)"gfx/arnoud.bmp",		0, 1, 1, 1},
	{(char *)"JOOST", 		(char *)"gfx/joost.bmp",		1, 0, 2, 1},
	{(char *)"KEES", 		(char *)"gfx/kees.bmp",			1, 2, 0, 1},
	{(char *)"BRYAN", 		(char *)"gfx/bryan.bmp",		1, 1, 0, 2},
	{(char *)"MARK", 		(char *)"gfx/mark.bmp", 		2, 0, 1, 1},
	{(char *)"SIM", 		(char *)"gfx/sim.bmp", 			2, 2, 0, 0},
};
#else
const Character Player::CHARACTERS[Player::CHARACTER_COUNT] = {
	//      Name               Filename            sp wt wp bd
	{(char*)"BERT",		(char*)"gfx/bert.bmp",		1, 1, 1, 1},
	{(char*)"JEROEN",	(char*)"gfx/jeroen.bmp",	1, 1, 2, 0},
	{(char*)"STEVEN",	(char*)"gfx/steven.bmp",	0, 2, 0, 2},
	{(char*)"TEDJE",	(char*)"gfx/tedje.bmp",		1, 1, 2, 0},
	{(char*)"OKKE",		(char*)"gfx/okke.bmp",		2, 0, 2, 0},
	{(char*)"JEREMY",	(char*)"gfx/jeremy.bmp",	2, 0, 0, 2},
	{(char*)"MARCEL",	(char*)"gfx/marcel.bmp",	2, 0, 1, 1},
	{(char*)"JAY",		(char*)"gfx/jay.bmp",		0, 2, 1, 1},
	{(char*)"DONJA",	(char*)"gfx/donja.bmp",		2, 0, 1, 1},
	{(char*)"ROB",		(char*)"gfx/rob.bmp",		0, 2, 0, 2},
	{(char*)"EVA",		(char*)"gfx/eva.bmp",		0, 2, 1, 1},
	{(char*)"TOBIAS",	(char*)"gfx/tobias.bmp",	1, 1, 0, 2},
	{(char*)"ARJAN",	(char*)"gfx/arjan.bmp",		0, 2, 2, 0},
	{(char*)"RUUD",		(char*)"gfx/ruud.bmp",		2, 0, 0, 2},
	{(char*)"PETER",	(char*)"gfx/peter.bmp",		1, 1, 1, 1},
	{(char*)"BERRY",	(char*)"gfx/berry.bmp",		2, 0, 1, 1},
	{(char*)"TON",		(char*)"gfx/ton.bmp",		0, 2, 2, 0},
//	{(char*)"ANTON",	(char*)"gfx/anton.bmp",		1, 1, 0, 2},
	{(char*)"GIJS",		(char*)"gfx/gijs.bmp",		1, 1, 0, 2},
	{(char*)"RUTGER",	(char*)"gfx/rutger.bmp",	2, 0, 2, 0},
	{(char*)"KIM",		(char*)"gfx/kim.bmp",		1, 1, 1, 1},
};
#endif

const int Player::COLORS_COUNT = 4;
const int Player::COLORS[4] = {
	0xaa0000,
	0x0000aa,
	0x009900,
	0xaa9900,
};

const short Player::SUIT_COLOR_COUNT = 5;
const Uint32 Player::SUIT_ORIGINAL[Player::SUIT_COLOR_COUNT] = {
	0x486d9f, // shirt
	0x395479, // shirt dark
	0x6d9360, // pants
	0x5e7855, // pants dark
	0x4c5f45 // pants darker
};

const Uint32 Player::SUIT_REPLACE[Player::COLORS_COUNT][Player::SUIT_COLOR_COUNT] = {
	//   shirt      dark     pants      dark    darker
	{ 0xb45454, 0x993b3b, 0x6d9360, 0x5e7855, 0x4c5f45 },
	{ 0x486d9f, 0x395479, 0x6d9360, 0x5e7855, 0x4c5f45 },
	{ 0x84c584, 0x71af71, 0x6d9360, 0x5e7855, 0x4c5f45 },
	{ 0xf0dd60, 0xd5c660, 0x6d9360, 0x5e7855, 0x4c5f45 }
};


const int Player::SPEEDCLASS_COUNT = 3;
const SpeedClass Player::SPEEDCLASSES[Player::SPEEDCLASS_COUNT] = {
	{30},
	{35},
	{40},
};
const int Player::WEIGHTCLASS_COUNT = 3;
const WeightClass Player::WEIGHTCLASSES[Player::WEIGHTCLASS_COUNT] = {
	{5,   5, 35, 20},
	{10, 10, 30, 15},
	{15, 15, 25, 10},
};
const int Player::WEAPONCLASS_COUNT = 3;
const WeaponClass Player::WEAPONCLASSES[Player::WEAPONCLASS_COUNT] = {
	{10, 300, 10},
	{ 9, 325, 10},
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

// Shield time
#define PLAYER_SHIELD_FRAMES 180

// Replace player suits with player color
#define PLAYER_REPLACE_SUIT_COLOR

Player::Player(int character, int number, Main &main) : main_(main) {
	name = CHARACTERS[character].name;
	this->character = character;
	this->number = number;
	is_spectating = false;
	
	update_suit();

	speedclass = CHARACTERS[character].speedclass;
	weightclass = CHARACTERS[character].weightclass;
	weaponclass = CHARACTERS[character].weaponclass;
	bombpowerclass = CHARACTERS[character].bombpowerclass;

	rounds_won = 0;

	input = NULL;

	reset();

	position = new SDL_Rect();
	last_position = new SDL_Rect();

	position->w = PLAYER_W;
	position->h = PLAYER_H;

	sprites = NULL;
	set_sprites();
	marker_clip_above = main_.graphics->pmarker_clip_above[suit_number];
	marker_clip_below = main_.graphics->pmarker_clip_below[suit_number];

}

Player::~Player() {
	delete position;
	delete last_position;
#ifdef PLAYER_REPLACE_SUIT_COLOR
	SDL_FreeSurface(sprites);
#endif
}

void Player::set_character(int character) {
	name = CHARACTERS[character].name;
	this->character = character;

	speedclass = CHARACTERS[character].speedclass;
	weightclass = CHARACTERS[character].weightclass;
	weaponclass = CHARACTERS[character].weaponclass;
	bombpowerclass = CHARACTERS[character].bombpowerclass;

	set_sprites();
}

void Player::set_sprites() {
	if (main_.no_sdl)
		return;
#ifdef PLAYER_REPLACE_SUIT_COLOR
	SDL_Surface * original;
	Uint32 c_old, n_new;

	if (sprites != NULL) {
		SDL_FreeSurface(sprites);
	}

	original = main_.graphics->player->at(character);
	sprites = SDL_ConvertSurface(original, original->format, original->flags);

	for (short i = 0; i < Player::SUIT_COLOR_COUNT; i++) {
		c_old = Player::SUIT_ORIGINAL[i];
		n_new = Player::SUIT_REPLACE[suit_number][i];
		if (c_old != n_new) {
			main_.graphics->replace_color(sprites, c_old, n_new);
		}
	}
#else
	sprites = main_.graphics->player->at(character);
#endif
}

void Player::update_suit()
{
	suit_number = number;
	if (main_.runmode != MainRunModes::ARCADE) {
		suit_number++;
	}
	suit_number = suit_number % Player::COLORS_COUNT;
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

	if(input != NULL)
		input->reset();

	is_hit = false;
	hit_start = 0;
	hit_delay = 30;
	hit_flicker_frame = 0;

	is_dead = is_spectating;
	dead_start = 0;

	is_frozen = false;
	freeze_start = 0;

	is_shielded = false;
	shield_start = 0;
	shield_frame = 0;

	bounce_direction_x = 0;
	bounce_direction_y = 0;

	shoot_start = 0;
	shoot_delay = 10;

	bomb_start = 0;
	bomb_delay = 30;

	bullets = 10;
	bombs = 3;
	mines = 0;
	doubledamagebullets = 0;
	instantkillbullets = 10;

	score = 0;
	hitpoints = is_spectating ? 0 : 100;

	bullets_fired = 0;
	bullets_hit = 0;
	bombs_fired = 0;
	bombs_hit = 0;
	headstomps = 0;

	cycle_direction = CYCLE_UP;
}

void Player::draw(SDL_Surface * screen, bool marker, int frames_processed) {

	SDL_Rect rect;
	SDL_Surface * sprites;

	rect.x = position->x;
	rect.y = position->y;

	sprites = this->sprites;

	// Dead players are not visible
	if(is_dead) {
		return;
	}

	// Check if player is hit and cycle between a show and a hide of the player to create
	// a flicker effect
	if(is_hit) {
		hit_flicker_frame = (hit_flicker_frame + frames_processed) % 10;
		if(hit_flicker_frame < 5)
			return;
	}

	if(is_shielded) {
		if((shield_start + PLAYER_SHIELD_FRAMES) - main_.gameplay().frame > 60) {
			sprites = main_.graphics->shield;
		} else {
			shield_frame = (shield_frame + frames_processed) % 10;
			if(shield_frame >= 5)
				sprites = main_.graphics->shield;
		}
	}

	if (main_.no_sdl)
		return;

	SDL_BlitSurface(sprites, main_.graphics->player_clip[current_sprite], screen, &rect);

	// If the player is going out the side of the screen, we want it to
	// appear on the other side.
	if(position->x >= WINDOW_WIDTH - PLAYER_W) {
		rect.x = position->x - WINDOW_WIDTH;
		rect.y = position->y;

		SDL_BlitSurface(sprites, main_.graphics->player_clip[current_sprite], screen, &rect);
	}
	if(position->x <= 0) {
		rect.x = position->x + WINDOW_WIDTH;
		rect.y = position->y;

		SDL_BlitSurface(sprites, main_.graphics->player_clip[current_sprite], screen, &rect);
	}

	// Show marker if the player is above the screen
	if(position->y + position->h <= 0) {
		rect.x = position->x + ((PLAYER_W - marker_clip_below->w) / 2);
		rect.y = 0;
		SDL_BlitSurface(main_.graphics->pmarker, marker_clip_below, screen, &rect);
	}

	// Show marker above the player
	if(marker) {
		rect.x = position->x + ((PLAYER_W - marker_clip_above->w) / 2);
		rect.y = position->y - marker_clip_above->h - 4;
		SDL_BlitSurface(main_.graphics->pmarker, marker_clip_above, screen, &rect);
	}

	// Show lag above player if server is active
	if (main_.runmode == MainRunModes::SERVER) {
		SDL_Surface *surf = main_.text->render_text_small_gray(format("lag %.2f #%d [%d]", server_util::get_lag_for(main_, *this), this->suit_number, (int)this->spectating()).c_str());
		rect.x = position->x + ((PLAYER_W - surf->w) / 2);
		rect.y = position->y - surf->h - 4;
		SDL_BlitSurface(surf, NULL, screen, &rect);
		SDL_FreeSurface(surf);
	}
	else if (main_.runmode == MainRunModes::CLIENT && main_.ingame_debug_visible) {
		SDL_Surface *surf = main_.text->render_text_small_gray(format("[%d] lag %.2f #%d [%d]", this->number, server_util::get_lag_for(main_, *this), this->suit_number, (int)this->spectating()).c_str());
		rect.x = position->x + ((PLAYER_W - surf->w) / 2);
		rect.y = position->y - surf->h - 4;
		SDL_BlitSurface(surf, NULL, screen, &rect);
		SDL_FreeSurface(surf);
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

void Player::move(Level * level) {
	int speedx, speedy;
	int maxx;
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
		if(main_.gameplay().frame > hit_start + hit_delay) {
			is_hit = false;
		}
	}

	if(is_frozen) {
		if(main_.gameplay().frame > freeze_start + PLAYER_FREEZE_FRAMES) {
			is_frozen = false;
		}
	}

	if(is_shielded) {
		if(main_.gameplay().frame > shield_start + PLAYER_SHIELD_FRAMES) {
			is_shielded = false;
		}
		if(main_.gameplay().frame == (shield_start + PLAYER_SHIELD_FRAMES - 60)) {
			main_.audio->play(SND_SHIELD, position->x + (position->w / 2));
		}
	}

	speedx = SPEED_HORIZ;

	
	// Are we running?
	if(input->is_pressed(A_RUN)) {
		is_running = true;
		//maxx = MAX_MOMENTUM_RUN;
		maxx = SPEEDCLASSES[speedclass].run_speed;
	} else {
		is_running = false;
		maxx = MAX_MOMENTUM_HORIZ;
	}
	
	// Are we ducking?
	if(input->is_pressed(A_DOWN)) {
		is_duck = true;
	}
	else {
		is_duck = false;
	}
	
	// Are we forced to being ducked?
	if(is_duck_forced) {
		if(main_.gameplay().frame - duck_force_start > DUCK_FORCE_FRAMES) {
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

	if(!is_frozen && input->is_pressed(A_LEFT)) {
		// Move more to the left
		if(momentumx > 0) momentumx -= MOMENTUM_INTERV_HORIZ;
		if(momentumx >= -maxx) momentumx -= MOMENTUM_INTERV_HORIZ;
		else momentumx += MOMENTUM_INTERV_HORIZ;
	}
	if(!is_frozen && input->is_pressed(A_RIGHT)) {
		// Move more to the right
		if(momentumx < 0) momentumx += MOMENTUM_INTERV_HORIZ;
		if(momentumx <= maxx) momentumx += MOMENTUM_INTERV_HORIZ;
		else momentumx -= MOMENTUM_INTERV_HORIZ;
	}
	if(is_frozen || (!input->is_pressed(A_LEFT) && !input->is_pressed(A_RIGHT))) {
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
		if(!input->is_pressed(A_LEFT) && !input->is_pressed(A_RIGHT)) {
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
			if(input->is_pressed(A_LEFT) && !input->is_pressed(A_RIGHT)) {
				if(is_duck)
					set_sprite(SPR_L_DUCK);
				else
					set_sprite(SPR_L_WALK1);
			}
			if(!input->is_pressed(A_LEFT) && input->is_pressed(A_RIGHT)) {
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
			if(input->is_pressed(A_RIGHT)) {
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
			if(input->is_pressed(A_LEFT)) {
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

	if(input->is_pressed(A_JUMP) && !is_falling && !is_jumping && !is_frozen) {
		// Start the jump
		momentumy = MAX_MOMENTUM_JUMP;
		is_jumping = true;

		main_.audio->play(SND_JUMP, position->x);
	}
	if(!input->is_pressed(A_JUMP) && is_jumping) {
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
		if(input->is_pressed(A_JUMP)) {
			main_.audio->play(SND_JUMP, position->x);
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
		duck_force_start = main_.gameplay().frame;
		momentumy = -10;
		if(damage(WEIGHTCLASSES[other->weightclass].headjump_damage)) {
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
	freeze_start = main_.gameplay().frame;
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

bool Player::damage(int damage) {
	if(is_shielded)
		return true;

	if(is_dead || is_hit)
		return false;

	is_hit = true;
	hit_start = main_.gameplay().frame;

	switch (main_.runmode) {
		default:
			hitpoints -= damage;
			if (hitpoints < 0)
				hitpoints = 0;
			break;
		case MainRunModes::SERVER:
			{
				hitpoints -= damage;
				if (hitpoints < 0)
					hitpoints = 0;

				network::CommandSetHitPoints points;
				points.data.time = SDL_GetTicks();
				points.data.client_id = number;
				points.data.hitpoints = hitpoints;

				main_.getServer().sendAll(points);
			}
			break;
		case MainRunModes::CLIENT:
			break;
	}

	return true;
}

void Player::process() {
	if(is_dead)
		return;

	if (spectating())
		return;

	if(input->is_pressed(A_SHOOT)) {
		if(main_.gameplay().frame > shoot_start + WEAPONCLASSES[weaponclass].rate &&
			(((bullets == BULLETS_UNLIMITED) ||  bullets > 0) ||
			((doubledamagebullets == BULLETS_UNLIMITED) ||  doubledamagebullets > 0) ||
			((instantkillbullets == BULLETS_UNLIMITED) ||  instantkillbullets > 0))) 
		{
			Projectile *proj = create_projectile_for_player(position->x, position->y);

			if (main_.runmode == MainRunModes::CLIENT && main_.getServerClient().isConnected())
			{
				network::CommandShotFired fire;
				fire.data.time = SDL_GetTicks();
				// this is discarded by server anyway, we cannot send for other clients
				fire.data.client_id = main_.getServerClient().getClientId(); 
				// current sprite determines direction of bullet
				fire.data.current_sprite = current_sprite;
				fire.data.x = position->x;
				fire.data.y = position->y;
				fire.data.distance_travelled = proj->distance_traveled;
				main_.getServerClient().send(fire);
			}
		}
	}
	if(input->is_pressed(A_BOMB)) {
		if(main_.gameplay().frame > bomb_start + bomb_delay &&
			(bombs > 0 || bombs == -1 || mines > 0 || mines == -1)) 
		{
			Bomb *newbomb = create_bomb_for_player(position->x, position->y);
			
			if (main_.runmode == MainRunModes::CLIENT && main_.getServerClient().isConnected())
			{
				network::CommandBombDropped bomb;
				bomb.data.time = SDL_GetTicks();
				// this is discarded by server anyway, we cannot send for other clients
				bomb.data.client_id = main_.getServerClient().getClientId(); 
				// current sprite determines direction of bullet
				bomb.data.current_sprite = current_sprite;
				bomb.data.x = newbomb->position->x;
				bomb.data.y = newbomb->position->y;
				main_.getServerClient().send(bomb);
			}
		}
	}
}

Projectile * Player::create_projectile_for_player(Sint16 x, Sint16 y)
{
	Projectile *pr = create_projectile(x, y);

	if(current_sprite >= SPR_L && current_sprite <= SPR_L_DUCK) {
		pr->position->x = x - pr->position->w - 1;
	} else {
		pr->position->x = x + position->w + 1;
	}
	if(is_duck)
		pr->position->y = y + 28;
	else
		pr->position->y = y + 8;

	return pr;
}

Projectile * Player::create_projectile(Sint16 x, Sint16 y)
{
	shoot_start = main_.gameplay().frame;
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

	pr = new Projectile(main_.graphics->weapons, clip_weapon, main_);
	pr->owner = this;
	if(instantkill)
		pr->damage = 100;
	else if(doubledamage)
		pr->damage = WEAPONCLASSES[weaponclass].damage * 2;
	else
		pr->damage = WEAPONCLASSES[weaponclass].damage;

	if(current_sprite >= SPR_L && current_sprite <= SPR_L_DUCK) {
		pr->speedx = -10;
		pr->position->x = x;
	} else {
		pr->speedx = 10;
		pr->position->x = x;
	}
	if(is_duck)
		pr->position->y = y;
	else
		pr->position->y = y;
	main_.gameplay().add_object(pr);

	pr->max_distance = WEAPONCLASSES[weaponclass].distance;
	
	if(instantkill)
		instantkillbullets--;
	else if(doubledamage)
		doubledamagebullets--;
//			else if(ruleset.doubledamagebullets != BULLETS_UNLIMITED)
//				bullets--;
	
	bullets_fired++;

	main_.audio->play(SND_SHOOT, pr->position->x);

	return pr;
}

Bomb* Player::create_bomb_for_player(Sint16 x, Sint16 y)
{
	Bomb *b = create_bomb(x, y);

	b->position->x = x + (position->w - b->position->w) / 2;
	b->position->y = y + (position->h - b->position->h);

	return b;
}

Bomb* Player::create_bomb(Sint16 x, Sint16 y)
{
	bomb_start = main_.gameplay().frame;

	Bomb * b;

	if(mines > 0 || mines == -1)
		b = new Mine(main_.graphics->bombs, main_);
	else
		b = new Bomb(main_.graphics->bombs, main_);

	b->damage = BOMBPOWERCLASSES[bombpowerclass].damage;
	b->time = 60;
	b->frame_start = main_.gameplay().frame;
	b->frame_change_start = main_.gameplay().frame;
	b->frame_change_count = 12;
	b->owner = this;
	b->position->x = x;
	b->position->y = y;
	main_.gameplay().add_object(b);
	
	if(mines > 0) {
		mines -= 1;
	} else if(mines != -1) {
		if(bombs != -1)
			bombs -= 1;
	}

	bombs_fired++;

	main_.audio->play(SND_SHOOT, b->position->x);

	return b;
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

void Player::spectate(bool set)
{
	is_spectating = set;
}

bool Player::spectating()
{
	return is_spectating;
}