#include "SDL/SDL.h"

#include "Gameplay.h"
#include "Bomb.h"

const int Bomb::FRAME_COUNT = 5;
const int Bomb::FRAME_NORMAL = 0;
const int Bomb::FRAME_FLASH = 1;
const int Bomb::FRAME_STRIKE_NORMAL = 2;
const int Bomb::FRAME_STRIKE_FLASH = 3;
const int Bomb::FRAME_EXPLOSION = 4;

Bomb::Bomb() {
	damage = 0;
	speedy = 0;

	exploded = false;
	done = false;

	hit_on_impact = false;

	position = new SDL_Rect();
}

Bomb::Bomb(SDL_Surface * surface) {
	damage = 0;
	time = 180;
	speedy = 0;

	exploded = false;
	done = false;

	hit_on_impact = false;

	sprite = surface;

	position = new SDL_Rect();
	position->x = 0;
	position->y = 0;
	position->w = BOMB_W;
	position->h = BOMB_H;

	current_frame = 0;

	set_clips();
}

Bomb::~Bomb() {
	delete position;
	for(int i = 0; i < FRAME_COUNT; i++) {
		delete clip[i];
	}
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

	if(p == owner)
		return;

	if(p->is_hit)
		return;

	explode();
}

void Bomb::hit_npc(NPC * npc) {
	if(exploded)
		return;

	if(npc->is_hit)
		return;

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
				if(hit_on_impact)
					explode();
			}
			if(position->y > WINDOW_HEIGHT)
				done = true;
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
			if(hit_on_impact)
				current_frame = current_frame == FRAME_STRIKE_NORMAL ? FRAME_STRIKE_FLASH : FRAME_STRIKE_NORMAL;
			else
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

	Main::audio->play(SND_EXPLODE, position->x);

	Player * p;
	NPC * npc;
	SDL_Rect * rect_bomb;
	SDL_Rect * rect_player = NULL;
	SDL_Rect * rect_npc = NULL;
	bool player_hit = false;
	bool npcs_hit = false;

	rect_bomb = get_damage_rect();

	for(unsigned int i = 0; i < Gameplay::instance->players->size(); i++) {
		p = Gameplay::instance->players->at(i);
		if(p == owner)
			continue;

		rect_player = p->get_rect();

		if(Gameplay::is_intersecting(rect_bomb, rect_player)) {
			if(p->damage(damage))
				player_hit = true;
		}

		delete rect_player;
	}

	for(unsigned int i = 0; i < Gameplay::instance->npcs->size(); i++) {
		npc = Gameplay::instance->npcs->at(i);

		rect_npc = npc->get_rect();

		if(Gameplay::is_intersecting(rect_bomb, rect_npc)) {
			if(npc->damage(damage))
				npcs_hit = true;
		}

		delete rect_npc;
	}

	if(player_hit || npcs_hit)
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
	clip[FRAME_NORMAL]->w = BOMB_W;
	clip[FRAME_NORMAL]->h = BOMB_H;
	
	clip[FRAME_FLASH] = new SDL_Rect();
	clip[FRAME_FLASH]->x = BOMB_W;
	clip[FRAME_FLASH]->y = 0;
	clip[FRAME_FLASH]->w = BOMB_W;
	clip[FRAME_FLASH]->h = BOMB_H;

	clip[FRAME_STRIKE_NORMAL] = new SDL_Rect();
	clip[FRAME_STRIKE_NORMAL]->x = BOMB_W * 2;
	clip[FRAME_STRIKE_NORMAL]->y = 0;
	clip[FRAME_STRIKE_NORMAL]->w = BOMB_W;
	clip[FRAME_STRIKE_NORMAL]->h = BOMB_H;
	
	clip[FRAME_STRIKE_FLASH] = new SDL_Rect();
	clip[FRAME_STRIKE_FLASH]->x = BOMB_W * 3;
	clip[FRAME_STRIKE_FLASH]->y = 0;
	clip[FRAME_STRIKE_FLASH]->w = BOMB_W;
	clip[FRAME_STRIKE_FLASH]->h = BOMB_H;
	
	clip[FRAME_EXPLOSION] = new SDL_Rect();
	clip[FRAME_EXPLOSION]->x = 0;
	clip[FRAME_EXPLOSION]->y = BOMB_H;
	clip[FRAME_EXPLOSION]->w = 86;
	clip[FRAME_EXPLOSION]->h = 68;

	explosion_offset_x = (clip[FRAME_NORMAL]->w - clip[FRAME_EXPLOSION]->w) / 2;
	explosion_offset_y = -clip[FRAME_EXPLOSION]->h + clip[FRAME_NORMAL]->h;
}
