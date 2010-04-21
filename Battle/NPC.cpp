#include "SDL/SDL.h"

#include "Gameplay.h"

#include "NPC.h"

#define MOMENTUM_INTERV_HORIZ 1
#define MOMENTUM_INTERV_VERT 1

#define MAX_MOMENTUM_FALL 100

#define FRAME_CYCLE_DISTANCE 24

NPC::NPC() {
	done = false;

	is_stationary = false;

	frame_w = 0;
	frame_h = 0;
	current_sprite = 0;

	sprites = NULL;

	frame_idle = 0;

	frame_walk_first = 1;
	frame_walk_last = 3;

	frame_jump_first = 8;
	frame_jump_last = 8;

	frame_brake_first = 7;
	frame_brake_last = 7;

	frame_dead = 0;

	is_dead = false;

	max_speed = 20;
	jump_height = 20;

	bounce_weight = 20;

	move_direction = 0;

	momentumx = 0;
	momentumy = 0;

	is_jumping = false;
	is_falling = false;
	
	is_hit = false;
	hit_start = 0;
	hit_delay = 20;
	hit_flicker_frame = 0;

	is_frozen = false;
	freeze_start = 0;
	freeze_delay = 20;

	is_dead = false;
	dead_start = 0;

	hitpoints = 20;
	
	distance_walked = 0;
	jump_cycle_start = 0;
	cycle_direction = 0;

	position = new SDL_Rect();
	last_position = new SDL_Rect();
}

NPC::~NPC() {
	delete position;
	delete last_position;
}

SDL_Rect * NPC::get_rect() {
	SDL_Rect * rect;
	
	rect = new SDL_Rect();
	rect->x = position->x;
	rect->y = position->y;
	rect->w = position->w;
	rect->h = position->h;

	return rect;
}

void NPC::draw(SDL_Surface * screen) {
	SDL_Rect rect, rect_s;

	if(sprites == NULL)
		return;
	
	// Dead NPCs are not visible
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
	
	rect.x = position->x;
	rect.y = position->y;

	rect_s.x = (current_sprite % frames) * frame_w;
	rect_s.y = (current_sprite / frames) * frame_h;
	rect_s.w = frame_w;
	rect_s.h = frame_h;

	SDL_BlitSurface(sprites, &rect_s, screen, &rect);
	
	// If the NPC is going out the side of the screen, we want it to
	// appear on the other side.
	if(position->x >= WINDOW_WIDTH - frame_w) {
		rect.x = position->x - WINDOW_WIDTH;
		rect.y = position->y;
		SDL_BlitSurface(sprites, &rect_s, screen, &rect);
	}
	if(position->x <= 0) {
		rect.x = position->x + WINDOW_WIDTH;
		rect.y = position->y;
		SDL_BlitSurface(sprites, &rect_s, screen, &rect);
	}
}

void NPC::process() {
	if(!is_dead && hitpoints <= 0) {
		is_dead = true;
		dead_start = Gameplay::frame;
		set_sprite(frame_dead);
	}
	if(is_dead && (Gameplay::frame - dead_start >= 30)) {
		done = true;
	}
}

void NPC::reset() {
	set_sprite(frame_idle);
}

void NPC::bounce_up(SDL_Rect * source) {
	if(is_stationary) return;

	is_falling = true;
	is_frozen = true;
	freeze_start = Gameplay::frame;
	momentumy = 30;
	
	if(position->x < source->x) {
		momentumx -= 15;
	}
	if(position->x > source->x) {
		momentumx += 15;
	}

	if(momentumx > max_speed)
		momentumx = max_speed;
	if(momentumx < -max_speed)
		momentumx = -max_speed;
}

void NPC::bounce(Player * other) {
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

	// NPCs hit each others top
	if(is_above) {
		bounce_direction_y = -1;
		momentumy = 30;

		other->bounce_direction_y = 1;
		other->is_duck_forced = true;
		other->duck_force_start = Gameplay::frame;
		other->momentumy = -10;
		other->damage(5);

		hit_player_top_bottom(other);
	}
	if(is_below) {
		bounce_direction_y = 1;
		momentumy = -10;
		damage(Player::WEIGHTCLASSES[other->weightclass].headjump_damage);

		other->bounce_direction_y = -1;
		if(other->input->is_pressed(A_JUMP)) {
			Main::audio->play(SND_JUMP, position->x);
			other->momentumy = 40;
			other->is_falling = false;
			other->is_jumping = true;
		}
		other->momentumy = 30;
		
		hit_player_top_bottom(other);
	}
	if(!is_above && !is_below) {
		bounce_direction_y = 0;
	}

	newmomentumx = momentumx;
	other->newmomentumx = other->momentumx;
	// NPCs hit each others side
	if(is_left || is_right) {
		newmomentumx = other->momentumx;
		
		if(is_left) {
			bounce_direction_x = -1;
			newmomentumx -= (30 - bounce_weight);
			if(move_direction == 1) move_direction = -1;

			other->bounce_direction_x = 1;
			other->newmomentumx = other->newmomentumx + 10 + bounce_weight;
		}
		if(is_right) {
			bounce_direction_x = 1;
			newmomentumx += (30 - bounce_weight);
			if(move_direction == -1) move_direction = 1;
			
			other->bounce_direction_x = -1;
			other->newmomentumx = other->newmomentumx - 10 -bounce_weight;
		}

		hit_player_side(other);
	} else {
		bounce_direction_x = 0;
	}
}

void NPC::bounce(NPC * other) {
	if(is_stationary) return;

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

	// NPCs hit each others top
	if(is_above) {
		bounce_direction_y = -1;
		momentumy = 30;
	}
	if(is_below) {
		bounce_direction_y = 1;
		/*
		if(!is_hit) {
			is_hit = true;
			hit_start = Gameplay::frame;
			momentumy = -10;
			hitpoints -= WEIGHTCLASSES[other->weightclass].headjump_damage;
		}*/
	}
	if(!is_above && !is_below) {
		bounce_direction_y = 0;
	}

	newmomentumx = momentumx;
	// NPCs hit each others side
	if(is_left || is_right) {
		newmomentumx = other->momentumx;
		
		if(is_left) {
			bounce_direction_x = -1;
			newmomentumx -= (other->bounce_weight - bounce_weight);
			if(move_direction == 1) move_direction = -1;
		}
		if(is_right) {
			bounce_direction_x = 1;
			newmomentumx += (other->bounce_weight - bounce_weight);
			if(move_direction == -1) move_direction = 1;
		}
	} else {
		bounce_direction_x = 0;
	}
}

bool NPC::damage(int damage) {
	if(is_dead || is_hit)
		return false;

	is_hit = true;
	hit_start = Gameplay::frame;

	hitpoints -= damage;

	if(hitpoints < 0)
		hitpoints = 0;

	return true;
}

void NPC::move(Level * level) {
	int speedx, speedy;
	SDL_Rect rect;

	speedx = 0;
	speedy = 0;

	if(is_dead)
		return;
	
	last_position->x = position->x;
	last_position->y = position->y;
	last_position->w = position->w;
	last_position->h = position->h;
	
	if(is_hit) {
		// The player has been hit long enough
		if(Gameplay::frame > hit_start + hit_delay) {
			is_hit = false;
		}
	}
	
	if(is_frozen) {
		if(Gameplay::frame > freeze_start + freeze_delay) {
			is_frozen = false;
		}
	}

	if(is_stationary) {
		momentumx = 0;
		newmomentumx = 0;
		return;
	}

	speedx = 1;

	// Movement calculation
	if(!is_frozen && move_direction == -1) {
		if(momentumx > 0) momentumx -= MOMENTUM_INTERV_HORIZ;
		if(momentumx >= -max_speed) momentumx -= MOMENTUM_INTERV_HORIZ;
		else momentumx += MOMENTUM_INTERV_HORIZ;
	}
	if(!is_frozen && move_direction == 1) {
		if(momentumx < 0) momentumx += MOMENTUM_INTERV_HORIZ;
		if(momentumx <= max_speed) momentumx += MOMENTUM_INTERV_HORIZ;
		else momentumx -= MOMENTUM_INTERV_HORIZ;
	}
	if(is_frozen || move_direction == 0) {
		if(momentumx < 0) momentumx += MOMENTUM_INTERV_HORIZ;
		if(momentumx > 0) momentumx -= MOMENTUM_INTERV_HORIZ;
	}
	
	// Do the move horizontally
	speedx = (int)((double)speedx * ((double)momentumx / 10));
	position->x += speedx;
	
	// Which sprite do we want to show?
	if(momentumx == 0) {
		// Standing still
		if(move_direction == 0) {
			if(current_sprite >= frames) {
				if(is_jumping || is_falling) {
					cycle_sprite_updown(frame_jump_first + frames, frame_jump_last + frames);
				} else {
					set_sprite(frame_idle + frames);
				}
			}
			if(current_sprite < frames) {
				if(is_jumping || is_falling) {
					cycle_sprite_updown(frame_jump_first, frame_jump_last);
				} else {
					set_sprite(frame_idle);
				}
			}
		}
		else {
			if(move_direction == -1) {
				set_sprite(frame_idle + frames);
			}
			if(move_direction == 1) {
				set_sprite(frame_idle);
			}
		}
		distance_walked = 0;
	}
	if(momentumx < 0) {
		// Moving left
		if(is_jumping || is_falling) {
			if(current_sprite < (frame_jump_first + frames) || current_sprite > (frame_jump_last + frames)) {
				cycle_sprite_updown(frame_jump_first + frames, frame_jump_last + frames);
				jump_cycle_start = Gameplay::instance->frame;
			} else {
				if(Gameplay::instance->frame - jump_cycle_start >= 6) {
					cycle_sprite_updown(frame_jump_first + frames, frame_jump_last + frames);
					jump_cycle_start = Gameplay::instance->frame;
				}
			}
		} else {
			if(current_sprite < (frame_walk_first + frames) || current_sprite > (frame_walk_last + frames)) {
				cycle_sprite_updown(frame_walk_first + frames, frame_walk_last + frames);
			}
			if(move_direction == 1) {
				cycle_sprite_updown(frame_brake_first + frames, frame_brake_last + frames);
				distance_walked = 0;
			}
			if(distance_walked < -FRAME_CYCLE_DISTANCE) {
				cycle_sprite_updown(frame_walk_first + frames, frame_walk_last + frames);
				distance_walked = 0;
			}
			distance_walked += speedx;
		}
	}
	else if(momentumx > 0) {
		// Moving right
		if(is_jumping || is_falling) {
			if(current_sprite < (frame_jump_first) || current_sprite > (frame_jump_last)) {
				cycle_sprite_updown(frame_jump_first, frame_jump_last);
				jump_cycle_start = Gameplay::instance->frame;
			} else {
				if(Gameplay::instance->frame - jump_cycle_start >= 6) {
					cycle_sprite_updown(frame_jump_first, frame_jump_last);
					jump_cycle_start = Gameplay::instance->frame;
				}
			}
		} else {
			if(current_sprite < frame_walk_first || current_sprite > frame_walk_last) {
				cycle_sprite_updown(frame_walk_first, frame_walk_last);
			}
			if(move_direction == -1) {
				cycle_sprite_updown(frame_brake_first, frame_brake_last);
				distance_walked = 0;
			}
			if(distance_walked > FRAME_CYCLE_DISTANCE) {
				cycle_sprite_updown(frame_walk_first, frame_walk_last);
				distance_walked = 0;
			}
			distance_walked += speedx;
		}
	}
	
	rect.x = position->x;
	rect.y = position->y;
	rect.w = position->w;
	rect.h = position->h;
	
	if(level->is_intersecting(&rect)) {
		// Stop if colliding with the level
		position->x -= speedx;
		momentumx = 0;
		move_direction = -move_direction;
	}

	// Move through the sides
	// If we went too far to the right, appear at the far left (and vica versa)
	if(position->x >= WINDOW_WIDTH)
		position->x -= WINDOW_WIDTH;
	if(position->x < 0)
		position->x += WINDOW_WIDTH;

	
	// Vertical movement

	if(is_falling || is_jumping) {
		speedy = 2;
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
	
	// Move the NPC vertically
	position->y -= speedy;

	rect.x = position->x;
	rect.y = position->y;
	rect.w = position->w;
	rect.h = position->h;
	
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


void NPC::set_sprite(int sprite) {
	current_sprite = sprite;
}

void NPC::cycle_sprite(int first, int last) {
	if(current_sprite < first || current_sprite > last) {
		current_sprite = first;
		return;
	}
	if(current_sprite == last)
		current_sprite = first;
	else
		current_sprite++;
}

void NPC::cycle_sprite_updown(int first, int last) {
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

void NPC::hit_player_side(Player *p) {}
void NPC::hit_player_top_bottom(Player *p) {}
