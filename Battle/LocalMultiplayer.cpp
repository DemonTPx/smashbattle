#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"

#include "Main.h"
#include "Level.h"
#include "Gameplay.h"

#include "LocalMultiplayer.h"

void LocalMultiplayer::on_game_reset() {
	int x, y;
	int sprite;
	Player * p;
	for(unsigned int idx = 0; idx < players->size(); idx++) {
		p = players->at(idx);

		switch(idx) {
			case 0:
				x = 160 + ((TILE_W - p->position->w) / 2);
				y = 320 - p->position->h;
				sprite = SPR_R;
				break;
			case 1:
				x = 448 + ((TILE_W - p->position->w) / 2);
				y = 320 - p->position->h;
				sprite = SPR_L;
				break;
			case 2:
				x = 160 + ((TILE_W - p->position->w) / 2);
				y = 128 - p->position->h;
				sprite = SPR_R;
				break;
			case 3:
				x = 448 + ((TILE_W - p->position->w) / 2);
				y = 128 - p->position->h;
				sprite = SPR_L;
				break;
		}
		
		p->position->x = x;
		p->position->y = y;
		p->hitpoints = 100;
		p->shoot_start = 0;
		p->bomb_start = 0;
		p->is_duck_forced = false;
		p->duck_force_start = 0;
		p->is_hit = false;
		p->hit_start = 0;
		p->is_frozen = false;
		p->freeze_start = 0;
		//p->bullets = ruleset.bullets;
		//p->bombs = ruleset.bombs;
		//p->doubledamagebullets = ruleset.doubledamagebullets;
		//p->instantkillbullets = ruleset.instantkillbullets;
		p->is_falling = false;
		p->is_jumping = false;
		p->momentumx = 0;
		p->momentumy = 0;
		p->set_sprite(sprite);
	}
}

void LocalMultiplayer::on_pre_processing() {}

void LocalMultiplayer::on_post_processing() {
	if(!paused && !countdown && !ended) {
		// Check if a player lost
		Player * p, * p2;
		int playersleft;
		playersleft = 0;
		for(unsigned int idx = 0; idx < players->size(); idx++) {
			p = players->at(idx);
			if(p->hitpoints < 0) {
				Main::audio->play(SND_YOULOSE);

				if(p->hitpoints < 0)
					p->hitpoints = 0;
				p->score--;

				p->is_hit = true;
				for(unsigned int i2 = 0; i2 < players->size(); i2++) {
					p2 = players->at(i2);
					if(p == p2) continue;
					if(p2->hitpoints > 0) {
						p2->is_hit = false;
						playersleft++;
					}
				}

				if(playersleft <= 1) {
					ended = true;
					end_timer = new Timer();
					end_timer->start();
					end_avatar_start_frame = frame;
				}
			}
		}
	}
}
