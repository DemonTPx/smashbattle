#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"

#include "Timer.h"
#include "Main.h"

#include "Player.h"

#include "Battle.h"

#define SPR_W 32
#define SPR_H 32
#define SPR_COLS 20
#define SPR_ROWS 15
#define SPR_COUNT (20 * 15)

#define MAX_MOMENTUM_FALL 100
#define MAX_MOMENTUM_JUMP 40
#define MAX_MOMENTUM_HORIZ 30

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
   5, 5, 5, 5, 5,  5, 5, 5, 5, 5,  5, 5, 5, 5, 5,  5, 5, 5, 5, 5,
  -1,-1,-1,-1,-1, -1,-1,-1,-1,-1, -1,-1,-1,-1,-1, -1,-1,-1,-1,-1 };
  

Battle::Battle() {

}

Battle::~Battle() {

}

void Battle::run() {
	load_images();

	SDL_Event event;
	SDL_Surface * screen;
	int frame;

	screen = Main::instance->screen;

	game_running = true;

	SDL_FillRect(screen, NULL, 0);
	draw_level(screen);

	Player player1("bert.bmp");
	Player player2("jeroen.bmp");

	player1.position->x = 160;
	player1.position->y = 256;
	player1.key_l = SDLK_a;
	player1.key_r = SDLK_d;
	player1.key_u = SDLK_w;

	player2.position->x = 448;
	player2.position->y = 256;
	player2.key_l = SDLK_LEFT;
	player2.key_r = SDLK_RIGHT;
	player2.key_u = SDLK_UP;
	player2.set_sprite(SPR_L);

	frame = 0;
	paused = false;

	while (Main::running && game_running) {
		// Event handling
		while(SDL_PollEvent(&event)) {
			Main::instance->handle_event(&event);
			if(event.type == SDL_KEYDOWN) {
				if(event.key.keysym.sym == SDLK_ESCAPE) {
					paused = !paused;
				}
				if(event.key.keysym.sym == SDLK_n) {
					if(paused) paused = false;
				}
				if(event.key.keysym.sym == SDLK_y) {
					if(paused) game_running = false;
				}
			}
			player1.handle_input(&event);
			player2.handle_input(&event);
		}

		// Processing
		if(!paused) {
			move_player(&player1);
			move_player(&player2);
			check_player_collision(&player1, &player2);
		}
		
		// Drawing

		frame++;
		draw_level(screen);

		player1.show(screen);
		player2.show(screen);
		
		if(paused) {
			draw_pause_screen(screen);
		}

		// Flipping

		Main::instance->flip();
	}

	free_images();
}

void Battle::move_player(Player * p) {
	int speedx, speedy;
	int momentumx_old;
	
	speedx = 0;
	speedy = 0;

	speedx = SPEED_HORIZ;

	momentumx_old = p->momentumx;

	if(p->keydn_l) {
		// Move more to the left
		if(p->momentumx > -MAX_MOMENTUM_HORIZ) p->momentumx -= MOMENTUM_INTERV_HORIZ;
		if(p->momentumx > 0) p->momentumx -= MOMENTUM_INTERV_HORIZ;
	}
	if(p->keydn_r) {
		// Move more to the right
		if(p->momentumx < MAX_MOMENTUM_HORIZ) p->momentumx += MOMENTUM_INTERV_HORIZ;
		if(p->momentumx < 0) p->momentumx += MOMENTUM_INTERV_HORIZ;
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
			if(p->distance_walked < -16) {
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
			if(p->distance_walked > 16) {
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

void Battle::check_player_collision(Player * p1, Player * p2) {
	// Check if there is a collision between the players and process the further movement
	int l1, r1, t1, b1;
	int l2, r2, t2, b2;
	int momx1, momy1;
	int momx2, momy2;
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
	if(r1 <= l2) return;
	if(l1 >= r2) return;

	// Collision

	// Move players out of collision zone
	p1->position->x -= p1->last_speedx;
	p1->position->y += p1->last_speedy;

	p2->position->x -= p2->last_speedx;
	p2->position->y += p2->last_speedy;

	// Bounce back
	momx1 = p1->momentumx;
	momy1 = p1->momentumy;

	momx2 = p2->momentumx;
	momy2 = p2->momentumy;

	if(t1 < t2) { // p1 above p2
		diffy = b1 - t2;
		upper = p1;
		lower = p2;
	} else { // p1 below p2, or at the same level
		diffy = b2 - t1;
		upper = p2;
		lower = p1;
	}
	if(l1 < l2) { // p1 at the left of p2
		diffx = r1 - l2;
		left = p1;
		right = p2;
	} else { // p1 at the right of p2, or at the same level
		diffx = r2 - l1;
		left = p2;
		right = p1;
	}

	if(diffx > diffy) { // Players hit each others top
		upper->momentumy = 30;
		lower->momentumy = -10;
	} else { // Players hit each others side
		left->momentumx = -20;
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

bool Battle::check_collision(SDL_Rect * rect) {
	// Check if the rect is colliding with the level
	int l, r, t, b;

	l = rect->x;
	r = rect->x + rect->w - 1;

	t = rect->y;
	b = rect->y + rect->h - 1;

	if(l >= WINDOW_WIDTH) l = l - WINDOW_WIDTH;
	if(r >= WINDOW_WIDTH) r = r - WINDOW_WIDTH;

	if(t >= WINDOW_HEIGHT) t = t - WINDOW_HEIGHT;
	if(b >= WINDOW_HEIGHT) b = b - WINDOW_HEIGHT;

	if(t < 0) t = 0;
	if(b > WINDOW_HEIGHT) b = WINDOW_HEIGHT;

	for(int y = t; y < b; y++) {
		if(level[level_pos(l, y)] != -1)
			return true;
		if(level[level_pos(r, y)] != -1)
			return true;
	}

	for(int x = l; x < r; x++) {
		if(level[level_pos(x, t)] != -1)
			return true;
		if(level[level_pos(x, b)] != -1)
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

	if(l >= WINDOW_WIDTH) l = l - WINDOW_WIDTH;
	if(r >= WINDOW_WIDTH) r = r - WINDOW_WIDTH;

	if(t >= WINDOW_HEIGHT) t = t - WINDOW_HEIGHT;
	if(b >= WINDOW_HEIGHT) b = b - WINDOW_HEIGHT;
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

void Battle::draw_pause_screen(SDL_Surface * screen) {
	SDL_Surface * surface;
	SDL_Rect rect;

	surface = TTF_RenderText_Solid(font, "QUIT GAME Y/N", fontColor);
	rect.x = (screen->w - surface->w) / 2;
	rect.y = (screen->h - surface->h) / 2;

	SDL_BlitSurface(surface, NULL, screen, &rect);

	SDL_FreeSurface(surface);
}

void Battle::load_images() {
	SDL_Surface * surface;
	
	surface = SDL_LoadBMP("bg.bmp");
	background = SDL_DisplayFormat(surface);
	SDL_FreeSurface(surface);

	surface = SDL_LoadBMP("tiles.bmp");
	tiles = SDL_DisplayFormat(surface);
	SDL_FreeSurface(surface);

	set_clips();

	font = TTF_OpenFont("slick.ttf", 26);
	fontColor.r = 255;
	fontColor.g = 255;
	fontColor.b = 255;
}

void Battle::free_images() {
	SDL_FreeSurface(tiles);
	SDL_FreeSurface(background);

	delete * tile_rect;

	TTF_CloseFont(font);
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