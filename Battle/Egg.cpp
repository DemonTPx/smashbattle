#include "SDL/SDL.h"

#include "Gameplay.h"
#include "Egg.h"
#include "Main.h"

Egg::Egg(SDL_Surface * surface, Main &main) : Bomb(surface, main), main_(main) {
	position->w = EGG_W;
	position->h = EGG_H;

	set_clips();
}

void Egg::set_clips() {
	// Delete some inherited stuff
	delete clip[FRAME_NORMAL];
	delete clip[FRAME_FLASH];
	delete clip[FRAME_STRIKE_NORMAL];
	delete clip[FRAME_STRIKE_FLASH];
	delete clip[FRAME_EXPLOSION];

	clip[FRAME_NORMAL] = new SDL_Rect();
	clip[FRAME_NORMAL]->x = 48;
	clip[FRAME_NORMAL]->y = 0;
	clip[FRAME_NORMAL]->w = EGG_W;
	clip[FRAME_NORMAL]->h = EGG_H;

	clip[FRAME_FLASH] = new SDL_Rect();
	clip[FRAME_FLASH]->x = 48 + EGG_W;
	clip[FRAME_FLASH]->y = 0;
	clip[FRAME_FLASH]->w = EGG_W;
	clip[FRAME_FLASH]->h = EGG_H;

	clip[FRAME_STRIKE_NORMAL] = new SDL_Rect();
	clip[FRAME_STRIKE_NORMAL]->x = 48;
	clip[FRAME_STRIKE_NORMAL]->y = 0;
	clip[FRAME_STRIKE_NORMAL]->w = EGG_W;
	clip[FRAME_STRIKE_NORMAL]->h = EGG_H;

	clip[FRAME_STRIKE_FLASH] = new SDL_Rect();
	clip[FRAME_STRIKE_FLASH]->x = 48 + EGG_W;
	clip[FRAME_STRIKE_FLASH]->y = 0;
	clip[FRAME_STRIKE_FLASH]->w = EGG_W;
	clip[FRAME_STRIKE_FLASH]->h = EGG_H;

	clip[FRAME_EXPLOSION] = new SDL_Rect();
	clip[FRAME_EXPLOSION]->x = 72;
	clip[FRAME_EXPLOSION]->y = 0;
	clip[FRAME_EXPLOSION]->w = 32;
	clip[FRAME_EXPLOSION]->h = 12;

	explosion_offset_x = (clip[FRAME_NORMAL]->w - clip[FRAME_EXPLOSION]->w) / 2;
	explosion_offset_y = -clip[FRAME_EXPLOSION]->h + clip[FRAME_NORMAL]->h + 8;
}

void Egg::hit_player(Player * p) {
	if(exploded)
		return;

	p->hitpoints += abs(damage);

	if(p->hitpoints > 100)
		p->hitpoints = 100;

	main_.audio->play(SND_ITEM, position->x);

	exploded = true;
	done = true;
}

void Egg::explode() {
	if(exploded)
		return;

	exploded = true;

	main_.audio->play(SND_EGG_BREAK, position->x);
}
