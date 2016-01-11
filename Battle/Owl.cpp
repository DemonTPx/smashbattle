#include "SDL/SDL.h"

#include "Gameplay.h"
#include "Owl.h"
#include "Bomb.h"
#include "Main.h"
#include <math.h>
#include <assert.h>

const int Owl::ANIMATION_SPEED = 6;
const int Owl::BOMB_AREA_OFFSET = 20;
const int Owl::FRAME_COUNT = 2;
const int Owl::FRAME_NORMAL = 0;
const int Owl::FRAME_FLASH = 1;

Owl::Owl(Main &main) : GameplayObject(main), main_(main) {
    assert( ! "Construction of Owl with one argument is not implemented");
}

Owl::Owl(SDL_Surface * surface, Main &main) : GameplayObject(main), main_(main) {
    speedx = 6;
    speedy = 0;
    hit = false;
    direction = rand() % 2 == 1 ? 1 : -1;
    speedx *= direction;

    damage = 100;

    sprite = surface;

    position = new SDL_Rect();
    position->x = static_cast<Sint16>(direction == 1 ? WINDOW_WIDTH : -OWL_W);
    position->y = 0;
    position->w = OWL_W;
    position->h = OWL_H;

    current_frame = 0;
    frame_change_start = main_.gameplay().frame;

    set_clips();
}

Owl::~Owl() {
    delete position;
    for(int i = 0; i < FRAME_COUNT; i++) {
        delete clip[i];
    }
}

void Owl::move(Level * level) {
    position->x -= speedx;

    Sint16 v = static_cast<Sint16>(cos(((float) main_.gameplay().frame) / 4) * 20 + 20);
    position->y = v;

    if (direction == 1) {
        if (position->x + position->w < 0) {
            done = true;
        }
    }
    else {
        if (position->x > WINDOW_WIDTH) {
            done = true;
        }
    }
}

void Owl::process() {
    // One frame after the owl hit, we mark it as done
    if(hit) {
        done = true;
        return;
    }

    // Animate owl
    if(main_.gameplay().frame - frame_change_start >= ANIMATION_SPEED) {
        current_frame = current_frame == FRAME_NORMAL ? FRAME_FLASH : FRAME_NORMAL;
        frame_change_start = main_.gameplay().frame;
    }

    if (position->x > BOMB_AREA_OFFSET && position->x < WINDOW_WIDTH - BOMB_AREA_OFFSET) {
        if (main_.gameplay().frame % 10 == 0) {
            Bomb *b = new Bomb(main_.graphics->bombs, main_);
            b->position->x = position->x + static_cast<Sint16>(OWL_W / 2) - static_cast<Sint16>(BOMB_W / 2);
            b->position->y = position->y + position->h;
            b->speedy = 30;
            b->time = 900;
            b->damage = Player::BOMBPOWERCLASSES[owner->bombpowerclass].damage;
            b->owner = owner;
            b->frame_start = main_.gameplay().frame;
            b->frame_change_start = main_.gameplay().frame;
            b->frame_change_count = 12;
            b->hit_on_impact = true;
            b->current_frame = Bomb::FRAME_STRIKE_NORMAL;

            main_.gameplay().add_object(b);
        }
    }
}

void Owl::hit_player(Player * player) {
    if(hit)
        return;

    if(player == owner)
        return;

    if(player->damage(damage)) {
        hit = true;
    }
}

void Owl::hit_npc(NPC * npc) {
    if(hit)
        return;

    if(owner == NULL)
        return;

    if(npc->damage(damage)) {
        hit = true;
    }
}

void Owl::draw_impl(SDL_Surface * screen, int frames_processed) {
    SDL_Rect rect;

    rect.x = position->x;
    rect.y = position->y;

    SDL_BlitSurface(sprite, clip[current_frame], screen, &rect);
}

void Owl::set_clips() {
    clip[FRAME_NORMAL] = new SDL_Rect();
    clip[FRAME_NORMAL]->x = 0;
    clip[FRAME_NORMAL]->y = 0;
    clip[FRAME_NORMAL]->w = OWL_W;
    clip[FRAME_NORMAL]->h = OWL_H;

    clip[FRAME_FLASH] = new SDL_Rect();
    clip[FRAME_FLASH]->x = OWL_W;
    clip[FRAME_FLASH]->y = 0;
    clip[FRAME_FLASH]->w = OWL_W;
    clip[FRAME_FLASH]->h = OWL_H;
}
