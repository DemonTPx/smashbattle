#pragma once

#include "GameplayObject.h"

#define OWL_W 90
#define OWL_H 72

class Owl : public GameplayObject {
public:
    Owl(Main &main);
    Owl(SDL_Surface * surface, Main &main);
    ~Owl();

    SDL_Surface * sprite;
    SDL_Rect * clip;

    int speedx, speedy;
    bool hit;

    Player * owner;

    int damage;

    virtual void move(Level * level);
    virtual void process();

    virtual void hit_player(Player * player);
    virtual void hit_npc(NPC * npc);

protected:

    virtual void draw_impl(SDL_Surface * screen, int frames_processed = 0);

private:

    Main &main_;
};
