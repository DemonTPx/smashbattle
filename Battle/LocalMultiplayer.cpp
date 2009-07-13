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
				y = 96 - p->position->h;
				sprite = SPR_R;
				break;
			case 3:
				x = 448 + ((TILE_W - p->position->w) / 2);
				y = 96 - p->position->h;
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
		p->is_dead = false;
		p->dead_start = 0;
		p->is_frozen = false;
		p->freeze_start = 0;
		//p->bullets = ruleset.bullets;
		//p->bombs = ruleset.bombs;
		p->bombs = 3;
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

				if(p->hitpoints < 0) {
					p->hitpoints = 0;
					if(!p->is_dead) {
						p->is_dead = true;
						p->dead_start = Gameplay::frame;
					}
				}
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

void LocalMultiplayer::draw_score() {
	SDL_Rect rect;
	// Fill the background
	rect.x = 0;
	rect.y = WINDOW_HEIGHT - 32;
	rect.w = WINDOW_WIDTH;
	rect.h = 32;
	SDL_FillRect(screen, &rect, 0x222222);

	if(Gameplay::instance->players->size() == 2)
		draw_score_duel();
	else
		draw_score_multi();
}

void LocalMultiplayer::draw_score_duel() {
	SDL_Surface * surface;
	SDL_Rect rect;
	SDL_Rect rect_s;
	int ammount;

	Player * player1, * player2;
	
	player1 = Gameplay::instance->players->at(0);
	player2 = Gameplay::instance->players->at(1);

	// Health bar player 1
	rect.x = 3;
	rect.y = WINDOW_HEIGHT - 29;
	rect.w = 120;
	rect.h = 9;
	SDL_FillRect(screen, &rect, 0);

	rect_s.w = (int)(1.18 * player1->hitpoints);
	rect_s.h = 7;
	rect_s.x = 0;
	rect_s.y = 0;
	rect.x = 4;
	rect.y = WINDOW_HEIGHT - 28;
	SDL_BlitSurface(Main::instance->graphics->player1hp, &rect_s, screen, &rect);

	// Health bar player 2
	rect.x = WINDOW_WIDTH - 123;
	rect.y = WINDOW_HEIGHT - 29;
	rect.w = 120;
	rect.h = 9;
	SDL_FillRect(screen, &rect, 0);

	rect_s.w = (int)(1.18 * player2->hitpoints);
	rect_s.h = 7;
	rect_s.x = 118 - rect_s.w;
	rect_s.y = 0;
	rect.x = WINDOW_WIDTH - 3 - rect_s.w;
	rect.y = WINDOW_HEIGHT - 28;
	SDL_BlitSurface(Main::instance->graphics->player2hp, &rect_s, screen, &rect);

	char str[40];

	//sprintf_s(str, 40, "%s %d", player1->name, player1->bullets);
	//surface = TTF_RenderText_Solid(font26, str, fontColor);
	surface = TTF_RenderText_Solid(Main::instance->graphics->font26, player1->name, Main::instance->graphics->white);
	rect.x = 2;
	rect.y = WINDOW_HEIGHT - surface->h;
	SDL_BlitSurface(surface, NULL, screen, &rect);
	SDL_FreeSurface(surface);

	//sprintf_s(str, 40, "%s %d", player2->name, player2->bullets);
	//surface = TTF_RenderText_Solid(font26, str, fontColor);
	surface = TTF_RenderText_Solid(Main::instance->graphics->font26, player2->name, Main::instance->graphics->white);
	rect.x = WINDOW_WIDTH - surface->w - 2;
	rect.y = WINDOW_HEIGHT - surface->h;
	SDL_BlitSurface(surface, NULL, screen, &rect);
	SDL_FreeSurface(surface);

	// Show score
	sprintf_s(str, 40, "%02d-%02d", player1->score, player2->score);
	surface = TTF_RenderText_Solid(Main::instance->graphics->font52, str, Main::instance->graphics->white);
	rect.x = (WINDOW_WIDTH - surface->w) / 2;
	rect.y = WINDOW_HEIGHT - surface->h + 3;
	SDL_BlitSurface(surface, NULL, screen, &rect);
	SDL_FreeSurface(surface);

	// Show bomb ammount
	rect_s.x = 12;
	rect_s.y = 0;
	rect_s.w = 12;
	rect_s.h = 16;
	rect.x = 150;
	rect.y = 460;
	SDL_BlitSurface(Main::instance->graphics->bombs, &rect_s, screen, &rect);
	rect.x = 474;
	rect.y = 460;
	SDL_BlitSurface(Main::instance->graphics->bombs, &rect_s, screen, &rect);

	if(player1->bombs != -1) {
		sprintf_s(str, 2, "%01d", player1->bombs);
		surface = TTF_RenderText_Solid(Main::instance->graphics->font26, str, Main::instance->graphics->white);
		rect.x = 180 - surface->w;
		rect.y = 460;
		SDL_BlitSurface(surface, NULL, screen, &rect);
		SDL_FreeSurface(surface);
	}

	if(player2->bombs != -1) {
		sprintf_s(str, 2, "%01d", player2->bombs);
		surface = TTF_RenderText_Solid(Main::instance->graphics->font26, str, Main::instance->graphics->white);
		rect.x = 460;
		rect.y = 460;
		SDL_BlitSurface(surface, NULL, screen, &rect);
		SDL_FreeSurface(surface);
	}
	
	// Ammo type and ammount
	rect_s.x = 0;
	rect_s.y = 0;
	rect_s.w = 8;
	rect_s.h = 8;
	rect.x = 120;
	rect.y = 464;
//	if(ruleset.doubledamagebullets == BULLETS_UNLIMITED || player1->doubledamagebullets > 0) rect_s.x = 8;
//	if(ruleset.instantkillbullets == BULLETS_UNLIMITED || player1->instantkillbullets > 0) rect_s.x = 16;
	SDL_BlitSurface(Main::instance->graphics->weapons, &rect_s, screen, &rect);

	if(player1->instantkillbullets > 0) ammount = player1->instantkillbullets;
	else if(player1->doubledamagebullets > 0) ammount = player1->doubledamagebullets;
	else {
//		if(ruleset.doubledamagebullets != BULLETS_UNLIMITED) ammount = player1->bullets;
//		else ammount = 0;
	}

	if(ammount > 0) {
		sprintf_s(str, 3, "%01d", ammount);
		surface = TTF_RenderText_Solid(Main::instance->graphics->font26, str, Main::instance->graphics->white);
		rect.x = 144 - surface->w;
		rect.y = 460;
		SDL_BlitSurface(surface, NULL, screen, &rect);
		SDL_FreeSurface(surface);
	}

	rect_s.x = 0;
	rect_s.y = 0;
	rect_s.w = 8;
	rect_s.h = 8;
	rect.x = 514;
	rect.y = 464;
//	if(ruleset.doubledamagebullets == BULLETS_UNLIMITED || player2->doubledamagebullets > 0) rect_s.x = 8;
//	if(ruleset.instantkillbullets == BULLETS_UNLIMITED || player2->instantkillbullets > 0) rect_s.x = 16;
	SDL_BlitSurface(Main::instance->graphics->weapons, &rect_s, screen, &rect);

	if(player2->instantkillbullets > 0) ammount = player2->instantkillbullets;
	else if(player2->doubledamagebullets > 0) ammount = player2->doubledamagebullets;
	else {
//		if(ruleset.doubledamagebullets != BULLETS_UNLIMITED) ammount = player2->bullets;
//		else ammount = 0;
	}
	if(ammount > 0) {
		sprintf_s(str, 3, "%01d", ammount);
		surface = TTF_RenderText_Solid(Main::instance->graphics->font26, str, Main::instance->graphics->white);
		rect.x = 496;
		rect.y = 460;
		SDL_BlitSurface(surface, NULL, screen, &rect);
		SDL_FreeSurface(surface);
	}

	// Show player avatars
	rect.x = 220 - PLAYER_W;
	rect.y = 450;
	SDL_BlitSurface(player1->sprites, player1->clip[SPR_R], screen, &rect);

	rect.x = 420;
	rect.y = 450;
	SDL_BlitSurface(player2->sprites, player2->clip[SPR_L], screen, &rect);
}

void LocalMultiplayer::draw_score_multi() {

}
