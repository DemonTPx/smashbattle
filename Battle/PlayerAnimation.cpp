#include <SDL/SDL.h>

#include "Main.h"
#include "PlayerAnimation.h"

#define CYCLE_UP 0
#define CYCLE_DN 1

// Base speed
#define SPEED_HORIZ 2
#define SPEED_VERT 2

#define MAX_MOMENTUM_FALL 100
#define MAX_MOMENTUM_JUMP 40
#define MAX_MOMENTUM_HORIZ 20
#define MAX_MOMENTUM_HORIZ_DUCKJUMP 10

#define FRAME_CYCLE_DISTANCE 24

#define MOMENTUM_INTERV_HORIZ 1
#define MOMENTUM_INTERV_VERT 1

PlayerAnimation::PlayerAnimation(int character) {
	this->character = character;

	position = new SDL_Rect();
	position->w = PLAYER_W;
	position->h = PLAYER_H;
	sprites = Main::graphics->player->at(character);
	current_sprite = 0;

	momentumx = 0;

	direction = 0;
	is_walking = false;
	is_running = false;
	is_duck = false;

	current_sprite = 0;

	distance_walked = 0;
	total_distance_walked = 0;

	animate_in_place = true;
}

PlayerAnimation::~PlayerAnimation() {
	delete position;
}

void PlayerAnimation::set_character(int character) {
	this->character = character;
	sprites = Main::graphics->player->at(character);
}

void PlayerAnimation::move() {
	int speedx, speedy;
	int maxx;
	int momentumx_old;
	
	speedx = 0;
	speedy = 0;

	speedx = SPEED_HORIZ;

	momentumx_old = momentumx;
	
	// Are we running?
	if(is_running) {
		is_running = true;
		maxx = Player::SPEEDCLASSES[Player::CHARACTERS[character].speedclass].run_speed;
	} else {
		is_running = false;
		maxx = MAX_MOMENTUM_HORIZ;
	}

	/*
	if(is_duck) {
		if(is_jumping || is_falling) {
			// The player can move when jumping and ducking at the same time
			maxx = MAX_MOMENTUM_HORIZ_DUCKJUMP;
		} else {
			// The player cannot move when ducking and standing on the ground
			maxx = 0;
		}
	}*/

	if(is_walking) {
		momentumx += (MOMENTUM_INTERV_HORIZ * direction);
	} else {
		if(momentumx < 0) momentumx += MOMENTUM_INTERV_HORIZ;
		if(momentumx > 0) momentumx -= MOMENTUM_INTERV_HORIZ;
	}
	if(momentumx <= -maxx) momentumx += MOMENTUM_INTERV_HORIZ;
	if(momentumx >= maxx) momentumx -= MOMENTUM_INTERV_HORIZ;

	// Move the player horizontally
	speedx = (int)((double)speedx * ((double)momentumx / 10));
	if(!animate_in_place)
		position->x += speedx;

	// Which sprite do we want to show?
	if(momentumx == 0) {
		// Standing still
		if(!is_walking) {
			if(current_sprite >= SPR_L && current_sprite <= SPR_L_DUCK) {
				if(is_duck) {
					set_sprite(SPR_L_DUCK);
				/*} else if(is_jumping || is_falling) {
					set_sprite(SPR_L_JUMP);*/
				} else {
					set_sprite(SPR_L);
				}
			}
			if(current_sprite >= SPR_R && current_sprite <= SPR_R_DUCK) {
				if(is_duck) {
					set_sprite(SPR_R_DUCK);
				/*} else if(is_jumping || is_falling) {
					set_sprite(SPR_R_JUMP);*/
				} else {
					set_sprite(SPR_R);
				}
			}
		}
		else {
			if(direction < 0) {
				if(is_duck)
					set_sprite(SPR_L_DUCK);
				else
					set_sprite(SPR_L_WALK1);
			}
			if(direction > 0) {
				if(is_duck)
					set_sprite(SPR_R_DUCK);
				else
					set_sprite(SPR_R_WALK1);
			}
		}
		distance_walked = 0;
		total_distance_walked = 0;
	}
	if(momentumx < 0) {
		// Moving left
		if(is_duck) {
			set_sprite(SPR_L_DUCK);
		/*} else if((is_jumping || is_falling) && !is_duck) {
			set_sprite(SPR_L_JUMP);*/
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
			if(direction > 0) {
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
		total_distance_walked += speedx;
	}
	else if(momentumx > 0) {
		// Moving right
		if(is_duck) {
			set_sprite(SPR_R_DUCK);
		/*} else if(is_jumping || is_falling) {
			set_sprite(SPR_R_JUMP);*/
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
			if(direction < 0) {
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
		total_distance_walked += speedx;
	}
	
	/*
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
	}*/
}

void PlayerAnimation::draw(SDL_Surface * screen) {
	SDL_Rect rect;
	rect.x = position->x;
	rect.y = position->y;
	SDL_BlitSurface(sprites, Main::graphics->player_clip[current_sprite], screen, &rect);
}

void PlayerAnimation::set_sprite(int sprite) {
	current_sprite = sprite;
}

void PlayerAnimation::cycle_sprite(int first, int last) {
	if(current_sprite < first || current_sprite > last) {
		current_sprite = first;
		return;
	}
	if(current_sprite == last)
		current_sprite = first;
	else
		current_sprite++;
}

void PlayerAnimation::cycle_sprite_updown(int first, int last) {
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
