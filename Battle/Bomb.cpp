#include "SDL/SDL.h"

#include "Gameplay.h"
#include "Bomb.h"

const int Bomb::FRAME_COUNT = 3;
const int Bomb::FRAME_NORMAL = 0;
const int Bomb::FRAME_FLASH = 1;
const int Bomb::FRAME_EXPLOSION = 2;

Bomb::Bomb() {
	damage = 0;
	speedy = 0;

	exploded = false;
	done = false;

	position = new SDL_Rect();
}

Bomb::Bomb(SDL_Surface * surface) {
	damage = 0;
	time = 180;
	speedy = 0;

	exploded = false;
	done = false;

	sprite = surface;

	position = new SDL_Rect();
	position->x = 0;
	position->y = 0;
	position->w = 12;
	position->h = 16;

	current_frame = 0;

	set_clips();
}

Bomb::~Bomb() {
	delete position;
	delete clip[FRAME_NORMAL];
	delete clip[FRAME_FLASH];
	delete clip[FRAME_EXPLOSION];
}

void Bomb::draw(SDL_Surface * screen) {
	SDL_Rect rect;

	rect.x = position->x;
	rect.y = position->y;
	
	// Flicker explosion
	if(exploded) {
		if(current_frame != FRAME_EXPLOSION) {
			current_frame = FRAME_EXPLOSION;
			frame_change_start = 0;
			flicker_frame = 0;
		}
		flicker_frame++;
		if(flicker_frame >= 30)
			done = true;
		if(flicker_frame % 12 >= 6) return;
	}

	if(current_frame == FRAME_EXPLOSION) {
		rect.x = position->x + explosion_offset_x;
		rect.y = position->y + explosion_offset_y;
	}

	SDL_BlitSurface(sprite, clip[current_frame], screen, &rect);

	// If the bomb is going out the side of the screen, we want it to
	// appear on the other side.
	if(current_frame == FRAME_EXPLOSION) {
		rect.x = position->x + explosion_offset_x;
		rect.y = position->y + explosion_offset_y;
	}
	if(rect.x >= screen->w - clip[current_frame]->w) {
		rect.x = rect.x - screen->w;
		rect.y = rect.y;
		SDL_BlitSurface(sprite, clip[current_frame], screen, &rect);
	}
	else if(rect.x <= 0) {
		rect.x = rect.x + screen->w;
		rect.y = rect.y;
		SDL_BlitSurface(sprite, clip[current_frame], screen, &rect);
	}
}

void Bomb::hit_player(Player * p) {
	if(exploded)
		return;

	if(p != owner)
		explode();
}

void Bomb::move(Level * level) {
	int speed;

	if(!exploded) {
		// Move the bomb
		speed = (int)(speedy / 10);

		if(speed != 0) {
			position->y += speed;

			if(level->is_intersecting(position)) {
				position->y -= speed;
				speedy = 0;
			}
		}
		
		// Check if the bomb is on the floor, fall if not
		if(!level->is_on_bottom(position)) {
			if(speedy > -20)
				speedy += 2;
		}
	}
}

void Bomb::process() {
	if(!exploded) {
		// Animate bomb
		if(Gameplay::frame - frame_change_start >= frame_change_count) {
			current_frame = current_frame == FRAME_NORMAL ? FRAME_FLASH : FRAME_NORMAL;
			frame_change_start = Gameplay::frame;
		}

		// Explode
		if(Gameplay::frame - frame_start >= time) {
			explode();
		}
	}
}

void Bomb::explode() {
	if(exploded)
		return;

	exploded = true;

	Main::audio->play(SND_EXPLODE);

	Player * p;
	SDL_Rect * rect_bomb;
	SDL_Rect * rect_player = NULL;
	bool player_hit = false;

	rect_bomb = get_damage_rect();

	for(unsigned int i = 0; i < Gameplay::instance->players->size(); i++) {
		p = Gameplay::instance->players->at(i);
		if(p == owner)
			continue;

		rect_player = p->get_rect();

		if(Gameplay::is_intersecting(rect_bomb, rect_player)) {
			p->hitpoints -= damage;
			p->is_hit = true;
			p->hit_start = Gameplay::frame;
			player_hit = true;
		}

		delete rect_player;
	}

	if(player_hit)
		owner->bombs_hit++;

	// Tiles below are also to be damaged
	rect_bomb->h += TILE_H;

	Gameplay::instance->level->damage_tiles(rect_bomb, damage);

	delete rect_bomb;
}

SDL_Rect * Bomb::get_damage_rect() {
	SDL_Rect * rect;

	rect = new SDL_Rect();
	rect->x = position->x + explosion_offset_x;
	rect->y = position->y + explosion_offset_y;
	rect->w = clip[FRAME_EXPLOSION]->w;
	rect->h = clip[FRAME_EXPLOSION]->h;
	return rect;
}

void Bomb::set_clips() {
	clip[FRAME_NORMAL] = new SDL_Rect();
	clip[FRAME_NORMAL]->x = 0;
	clip[FRAME_NORMAL]->y = 0;
	clip[FRAME_NORMAL]->w = 12;
	clip[FRAME_NORMAL]->h = 16;
	
	clip[FRAME_FLASH] = new SDL_Rect();
	clip[FRAME_FLASH]->x = 12;
	clip[FRAME_FLASH]->y = 0;
	clip[FRAME_FLASH]->w = 12;
	clip[FRAME_FLASH]->h = 16;
	
	clip[FRAME_EXPLOSION] = new SDL_Rect();
	clip[FRAME_EXPLOSION]->x = 0;
	clip[FRAME_EXPLOSION]->y = 16;
	clip[FRAME_EXPLOSION]->w = 86;
	clip[FRAME_EXPLOSION]->h = 68;

	explosion_offset_x = (clip[FRAME_NORMAL]->w - clip[FRAME_EXPLOSION]->w) / 2;
	explosion_offset_y = -clip[FRAME_EXPLOSION]->h + clip[FRAME_NORMAL]->h;
}
