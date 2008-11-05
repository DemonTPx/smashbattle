#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"

#include "Main.h"

#include "Player.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

const int Player::jump_height = 144;

#define PLAYER_SURF_COLS 10
#define PLAYER_SURF_COUNT 20

Player::Player(const char * name, const int number, const char * sprite_file) {
	this->name = (char*)name;
	this->number = (int)number;

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

	/*
	key_r = SDLK_RIGHT;
	key_l = SDLK_LEFT;
	key_u = SDLK_UP;
	key_d = SDLK_DOWN;
	key_run = SDLK_RSHIFT;
	key_shoot = SDLK_RCTRL;

	joystick_idx = 0;
	js_btn_u = 0;
	js_btn_run = 0;
	js_btn_shoot = 1;
	js_btn_start = 2;
	*/
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

	shoot_start = 0;
	shoot_delay = 10;

	bullets = 10;
	bombs = 3;

	hitpoints = 100;
	score = 0;

	cycle_direction = CYCLE_UP;

	position = new SDL_Rect();
	load_images(sprite_file);
	set_clips();
}

Player::~Player() {
	for(int i = 0; i < PLAYER_SURF_COUNT; i++) {
		delete clip[i];
	}
	delete marker_clip;
	
	delete position;
	free_images();
}

void Player::show(SDL_Surface * screen) {
	SDL_Rect rect;

	rect.x = position->x;
	rect.y = position->y;

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
					else if(controls.use_axis_up && event->jaxis.value > Main::JOYSTICK_AXIS_THRESHOLD) {
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
					if(event->jbutton.button == controls.js_jump) {
						keydn_u = true;
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
					if(event->jbutton.button == controls.js_jump) {
						keydn_u = false;
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
