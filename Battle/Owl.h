#pragma once

#include "GameplayObject.h"

#define OWL_W 42
#define OWL_H 36

class Owl : public GameplayObject {
public:
    Owl(Main &main);
    Owl(SDL_Surface * surface, Main &main);
    ~Owl();

    SDL_Surface * sprite;
    SDL_Rect * clip[2];

    int speedx, speedy;
    bool hit;

    int current_frame;
    int frame_change_start;
    int frame_change_count;

    Player * owner;

    int damage;

    static const int FRAME_COUNT;
    static const int FRAME_NORMAL;
    static const int FRAME_FLASH;

    virtual void move(Level * level);
    virtual void process();

    virtual void hit_player(Player * player);
    virtual void hit_npc(NPC * npc);

protected:

    virtual void draw_impl(SDL_Surface * screen, int frames_processed = 0);

private:

    void set_clips();

    Main &main_;
};
