#include "SDL/SDL.h"

#include "Gameplay.h"
#include "Owl.h"
#include "Bomb.h"
#include "Main.h"
#include <math.h>
#include <stdio.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

Owl::Owl(Main &main) : GameplayObject(main), main_(main) {
    speedx = 6;
    speedy = 0;
    hit = false;

    damage = 100;

    position = new SDL_Rect();
    clip = new SDL_Rect();
}

Owl::Owl(SDL_Surface * surface, Main &main) : GameplayObject(main), main_(main) {
    speedx = 6;
    speedy = 0;
    hit = false;

    damage = 100;

    sprite = surface;

    position = new SDL_Rect();
    position->x = WINDOW_WIDTH;
    position->y = 0;
    position->w = OWL_W;
    position->h = OWL_H;

    clip = new SDL_Rect();
    clip->x = 0;
    clip->y = 0;
    clip->w = OWL_W;
    clip->h = OWL_H;
}

Owl::~Owl() {
    delete position;
    delete clip;
}

void Owl::move(Level * level) {
    position->x -= speedx;

    Sint16 v = static_cast<Sint16>(cos(((float) main_.gameplay().frame) / 4) * 20 + 20);
    std::cout << v << std::endl;
    position->y = v;

    if (position->x + position->w < 0) {
        done = true;
    }
}

void Owl::process() {
    // One frame after the owl hit, we mark it as done
    if(hit) {
        done = true;
        return;
    }

    if (main_.gameplay().frame % 10 == 0) {
        Bomb * b = new Bomb(main_.graphics->bombs, main_);
        b->position->x = position->x;
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

    SDL_BlitSurface(sprite, clip, screen, &rect);
}
