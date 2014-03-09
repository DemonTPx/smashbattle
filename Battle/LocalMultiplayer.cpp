#include "SDL/SDL.h"

#include "Main.h"
#include "Level.h"
#include "Gameplay.h"

#include "HealthPowerUp.h"
#include "AmmoPowerUp.h"
#include "DoubleDamagePowerUp.h"
#include "InstantKillBulletPowerUp.h"
#include "BombPowerUp.h"
#include "MinePowerUp.h"
#include "AirstrikePowerUp.h"
#include "LaserBeamPowerUp.h"
#include "ShieldPowerUp.h"
#include "RandomPowerUp.h"

#include "LocalMultiplayer.h"

#ifndef WIN32
#define sprintf_s snprintf
#endif

void LocalMultiplayer::set_countdown(bool countdown, int seconds)
{
	this->countdown = countdown;
	if (seconds != -1)
		this->countdown_sec_left = seconds; 
}

void LocalMultiplayer::initialize() {
	Gameplay::initialize();

	powerup_rate = 300;
	powerup_max = 2;

	powerup_health_rate = 10;
	powerup_bullet_rate = 0;
	powerup_doubledamage_rate = 6;
	powerup_instantkill_rate = 1;
	powerup_bomb_rate = 6;
	powerup_mine_rate = 6;
	powerup_airstrike_rate = 2;
	powerup_laserbeam_rate = 3;
	powerup_shield_rate = 4;
	if (main_.runmode == MainRunModes::SERVER)
		powerup_random_rate = 0; // currently server does not support random powerup yet
	else
		powerup_random_rate = 6;

	round = 0;
}

bool LocalMultiplayer::we_have_a_winner() 
{
	bool we_have_a_winner = false;
	if(round >= 5) {
		int highest = 0;
		for(unsigned int idx = 0; idx < players->size(); idx++) {
			auto *p = players->at(idx);
			if(p->score == highest) {
				we_have_a_winner = false;
			}
			if(p->score > highest) {
				highest = p->score;
				we_have_a_winner = true;
			}
		}
	}
	return we_have_a_winner;
}

void LocalMultiplayer::on_game_reset() 
{
	int x, y;
	int sprite;
	Player * p;
	int pstart;
	
	if(we_have_a_winner()) {
		game_running = false;
		strcpy(countdown_pre_text, "");
		return;
	}

	for(unsigned int idx = 0; idx < players->size(); idx++) {
		p = players->at(idx);

		pstart = (idx + round) % 4;

		x = level->playerstart[pstart].x * TILE_W + ((TILE_W - PLAYER_W) / 2);
		y = level->playerstart[pstart].y * TILE_H - PLAYER_H;
		sprite = (level->playerstart[pstart].facing_right ? SPR_R : SPR_L);
		
		p->position->x = x;
		p->position->y = y;
		p->shoot_start = 0;
		p->bomb_start = 0;
		p->is_duck_forced = false;
		p->duck_force_start = 0;
		p->is_hit = false;
		p->hit_start = 0;
		p->dead_start = 0;
		p->is_frozen = false;
		p->freeze_start = 0;
		p->is_shielded = false;
		p->shield_start = 0;
		p->bullets = -1;
		p->bombs = 3;
		p->mines = 0;
		p->doubledamagebullets = 0;
		p->instantkillbullets = 0;
		p->is_falling = false;
		p->is_jumping = false;
		p->momentumx = 0;
		p->momentumy = 0;
		p->set_sprite(sprite);
		p->hitpoints = 100;
		p->is_dead = false;
	}

	round++;

	winner = NULL;
	if(round == 5)
		strcpy(countdown_pre_text, "FINAL ROUND");
	else if(round > 5)
		strcpy(countdown_pre_text, "EXTRA ROUND");
	else
		sprintf_s(countdown_pre_text, 20, "ROUND %d", round);
}

void LocalMultiplayer::on_pre_processing() {}

void LocalMultiplayer::on_post_processing() {
	if(!countdown && !ended) {
		// Check if a player lost
		Player * p, * p2;
		int playersleft;
		playersleft = 0;
		for(unsigned int idx = 0; idx < players->size(); idx++) {
			p = players->at(idx);
			
			if(p->hitpoints < 0)
				p->hitpoints = 0;

			if(p->is_dead)
				continue;

			if(p->hitpoints <= 0) {
				main_.audio->play(SND_YOULOSE, p->position->x);

				p->is_dead = true;
				p->dead_start = Gameplay::frame;
				p->is_hit = true;

				for(unsigned int i2 = 0; i2 < players->size(); i2++) {
					p2 = players->at(i2);
					if(p == p2) continue;
					if(p2->hitpoints > 0) {
						p2->score++;
						playersleft++;
						winner = p2;
					}
				}

				if(playersleft <= 1) {
					ended = true;
					end_start = frame;
					end_avatar_start_frame = frame;
					draw = false;
					if(playersleft == 0)
						draw = true;
					else
						winner->is_hit = false;
				}
			}
		}

		generate_powerup();
	}
}

GameplayObject *LocalMultiplayer::generate_powerup(bool force) 
{
	if(!force) {
		if(rand() % powerup_rate != 0) return NULL;
	}

	int powerups;

	powerups = 0;
	for(unsigned int idx = 0; idx < objects->size(); idx++) {
		if(objects->at(idx)->is_powerup)
			powerups++;
	}

	if(powerups >= powerup_max)
		return NULL;

	int r;
	int row, col;
	GameplayObject * gpo, * obj;
	SDL_Rect * rect, * pos;
	bool done;

	int max, first, last;

	done = false;
	while(!done) {
		row = rand() % TILE_ROWS - 1;
		col = rand() % TILE_COLS;
		if(level->level[row * TILE_COLS + col] == -1 && level->level[(row + 1) * TILE_COLS + col] != -1) {
			done = true;
			for(unsigned int idx = 0; idx < objects->size(); idx++) {
				obj = objects->at(idx);
				if(!obj->is_powerup)
					continue;
				if(obj->position->x == (col * TILE_W) + 8 &&
					obj->position->y == (row * TILE_H) + 16) {
					done = false;
				}
			}
		}
	}

	pos = new SDL_Rect();
	pos->w = 16;
	pos->h = 16;
	pos->x = (col * TILE_W) + 8;
	pos->y = (row * TILE_H) + 16;

	max = powerup_health_rate + powerup_bullet_rate + powerup_doubledamage_rate +
		powerup_instantkill_rate + powerup_bomb_rate + powerup_mine_rate +
		powerup_airstrike_rate + powerup_laserbeam_rate + powerup_shield_rate +
		powerup_random_rate;
	
	r = rand() % max;
	
	rect = new SDL_Rect();
	rect->w = 16;
	rect->h = 16;

	gpo = NULL;

	first = 0;
	last = first + powerup_health_rate;
	if(r >= first && r < last) {
		gpo = new HealthPowerUp(main_.graphics->powerups, rect, pos, 25, main_);
	}

	first = last;
	last = first + powerup_bullet_rate;
	if(r >= first && r < last) {
		gpo = new AmmoPowerUp(main_.graphics->powerups, rect, pos, 20, main_);
	}

	first = last;
	last = first + powerup_doubledamage_rate;
	if(r >= first && r < last) {
		gpo = new DoubleDamagePowerUp(main_.graphics->powerups, rect, pos, 5, main_);
	}

	first = last;
	last = first + powerup_instantkill_rate;
	if(r >= first && r < last) {
		gpo = new InstantKillBulletPowerUp(main_.graphics->powerups, rect, pos, 1, main_);
	}

	first = last;
	last = first + powerup_bomb_rate;
	if(r >= first && r < last) {
		gpo = new BombPowerUp(main_.graphics->powerups, rect, pos, 1, main_);
	}

	first = last;
	last = first + powerup_mine_rate;
	if(r >= first && r < last) {
		gpo = new MinePowerUp(main_.graphics->powerups, rect, pos, 1, main_);
	}

	first = last;
	last = first + powerup_airstrike_rate;
	if(r >= first && r < last) {
		gpo = new AirstrikePowerUp(main_.graphics->powerups, rect, pos, main_);
	}

	first = last;
	last = first + powerup_laserbeam_rate;
	if(r >= first && r < last) {
		gpo = new LaserBeamPowerUp(main_.graphics->powerups, rect, pos, main_);
	}

	first = last;
	last = first + powerup_shield_rate;
	if(r >= first && r < last) {
		gpo = new ShieldPowerUp(main_.graphics->powerups, rect, pos, main_);
	}

	first = last;
	last = first + powerup_random_rate;
	if(r >= first && r < last) {
		gpo = new RandomPowerUp(main_.graphics->powerups, pos, main_);
		delete rect;
	}

	if(gpo != NULL) {
		static short unique_id = 0;
		gpo->set_id(unique_id);
		unique_id++; // doesn't matter if it rotates
		objects->push_back(gpo);
	} else {
		delete rect;
		delete pos;
	}

	return gpo;
}

void LocalMultiplayer::draw_score() {
	if (main_.no_sdl)
		return;

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
	if (main_.no_sdl)
		return;
	SDL_Surface * surface;
	SDL_Rect rect;
	SDL_Rect rect_s;
	int ammount;
	char str[40];

	Player * player1, * player2;
	
	player1 = players->at(0);
	player2 = players->at(1);
	
	// Show player avatars
	rect.x = 274 - PLAYER_W;
	rect.y = 450;
	SDL_BlitSurface(player1->sprites, main_.graphics->player_clip[SPR_R], screen, &rect);

	rect.x = 366;
	rect.y = 450;
	SDL_BlitSurface(player2->sprites, main_.graphics->player_clip[SPR_L], screen, &rect);

	// Health bar player 1
	rect.x = 2;
	rect.y = WINDOW_HEIGHT - 30;
	rect.w = 122;
	rect.h = 10;
	SDL_FillRect(screen, &rect, 0);

	rect_s.w = (int)(1.18 * player1->hitpoints);
	rect_s.h = 8;
	rect_s.x = 0;
	rect_s.y = 0;
	rect.x = 4;
	rect.y = WINDOW_HEIGHT - 28;
	SDL_BlitSurface(main_.graphics->player1hp, &rect_s, screen, &rect);

	// Health bar player 2
	rect.x = WINDOW_WIDTH - 124;
	rect.y = WINDOW_HEIGHT - 30;
	rect.w = 122;
	rect.h = 10;
	SDL_FillRect(screen, &rect, 0);

	rect_s.w = (int)(1.18 * player2->hitpoints);
	rect_s.h = 8;
	rect_s.x = 118 - rect_s.w;
	rect_s.y = 0;
	rect.x = WINDOW_WIDTH - rect_s.w - 4;
	rect.y = WINDOW_HEIGHT - 28;
	SDL_BlitSurface(main_.graphics->player2hp, &rect_s, screen, &rect);

	// Player names
	surface = main_.graphics->playername->at(player1->character);
	rect.x = 240 - surface->w;
	rect.y = 455;
	SDL_BlitSurface(surface, NULL, screen, &rect);

	surface = main_.graphics->playername->at(player2->character);
	rect.x = 400;
	rect.y = 455;
	SDL_BlitSurface(surface, NULL, screen, &rect);

	// Show score
	sprintf_s(str, 40, "%d-%d", player1->score, player2->score);
	surface = main_.text->render_text_large(str);
	rect.x = (WINDOW_WIDTH - surface->w) / 2;
	rect.y = WINDOW_HEIGHT - surface->h - 2;
	SDL_BlitSurface(surface, NULL, screen, &rect);
	SDL_FreeSurface(surface);

	// Show bomb ammount
	if(player1->mines > 0 || player1->mines == -1) {
		rect_s.x = 54;
		rect_s.y = 0;
		rect_s.w = 6;
		rect_s.h = 4;
		rect.x = 6;
		rect.y = 472;
	} else {
		rect_s.x = 12;
		rect_s.y = 0;
		rect_s.w = 12;
		rect_s.h = 16;
		rect.x = 2;
		rect.y = 462;
	}
	SDL_BlitSurface(main_.graphics->bombs, &rect_s, screen, &rect);

	if(player2->mines > 0 || player2->mines == -1) {
		rect_s.x = 54;
		rect_s.y = 0;
		rect_s.w = 6;
		rect_s.h = 4;
		rect.x = 628;
		rect.y = 472;
	} else {
		rect_s.x = 12;
		rect_s.y = 0;
		rect_s.w = 12;
		rect_s.h = 16;
		rect.x = 626;
		rect.y = 462;
	}
	SDL_BlitSurface(main_.graphics->bombs, &rect_s, screen, &rect);

	rect_s.x = 0;
	rect_s.y = 0;
	rect_s.w = 16;
	rect_s.h = 16;

	if(player1->bombs != -1 && player1->mines != -1) {
		if(player1->mines > 0)
			sprintf_s(str, 3, "%02d", player1->mines);
		else
			sprintf_s(str, 3, "%02d", player1->bombs);
		surface = main_.text->render_text_medium(str);
		rect.x = 18;
		rect.y = 464;
		SDL_BlitSurface(surface, NULL, screen, &rect);
		SDL_FreeSurface(surface);
	} else {
		rect.x = 18;
		rect.y = 462;
		SDL_BlitSurface(main_.graphics->common, &rect_s, screen, &rect);
	}

	if(player2->bombs != -1 && player2->mines != -1) {
		if(player2->mines > 0)
			sprintf_s(str, 3, "%02d", player2->mines);
		else
			sprintf_s(str, 3, "%02d", player2->bombs);
		surface = main_.text->render_text_medium(str);
		rect.x = 622 - surface->w;
		rect.y = 464;
		SDL_BlitSurface(surface, NULL, screen, &rect);
		SDL_FreeSurface(surface);
	} else {
		rect.x = 622 - main_.graphics->common->w;
		rect.y = 462;
		SDL_BlitSurface(main_.graphics->common, &rect_s, screen, &rect);
	}
	
	// Ammo type and ammount
	rect_s.x = 0;
	rect_s.y = 0;
	rect_s.w = 8;
	rect_s.h = 8;
	rect.x = 62;
	rect.y = 466;
	if(player1->doubledamagebullets > 0) rect_s.x = 8;
	if(player1->instantkillbullets > 0) rect_s.x = 16;
	SDL_BlitSurface(main_.graphics->weapons, &rect_s, screen, &rect);

	if(player1->instantkillbullets > 0) ammount = player1->instantkillbullets;
	else if(player1->doubledamagebullets > 0) ammount = player1->doubledamagebullets;
	else ammount = -1;

	if(ammount != -1) {
		sprintf_s(str, 3, "%02d", ammount);
		surface = main_.text->render_text_medium(str);
		rect.x = 74;
		rect.y = 464;
		SDL_BlitSurface(surface, NULL, screen, &rect);
		SDL_FreeSurface(surface);
	} else {
		rect_s.x = 0;
		rect_s.y = 0;
		rect_s.w = 16;
		rect_s.h = 16;
		rect.x = 74;
		rect.y = 462;
		SDL_BlitSurface(main_.graphics->common, &rect_s, screen, &rect);
	}

	rect_s.x = 0;
	rect_s.y = 0;
	rect_s.w = 8;
	rect_s.h = 8;
	rect.x = 570;
	rect.y = 466;
	if(player2->doubledamagebullets > 0) rect_s.x = 8;
	if(player2->instantkillbullets > 0) rect_s.x = 16;
	SDL_BlitSurface(main_.graphics->weapons, &rect_s, screen, &rect);

	if(player2->instantkillbullets > 0) ammount = player2->instantkillbullets;
	else if(player2->doubledamagebullets > 0) ammount = player2->doubledamagebullets;
	else ammount = -1;

	if(ammount != -1) {
		sprintf_s(str, 3, "%02d", ammount);
		surface = main_.text->render_text_medium(str);
		rect.x = 566 - surface->w;
		rect.y = 464;
		SDL_BlitSurface(surface, NULL, screen, &rect);
		SDL_FreeSurface(surface);
	} else {
		rect_s.x = 0;
		rect_s.y = 0;
		rect_s.w = 16;
		rect_s.h = 16;
		rect.x = 566 - 16;
		rect.y = 462;
		SDL_BlitSurface(main_.graphics->common, &rect_s, screen, &rect);
	}
}

void LocalMultiplayer::draw_score_multi() {
	if (main_.no_sdl)
		return;
	SDL_Surface * surface;
	SDL_Rect rect;
	SDL_Rect rect_s;
	int ammount;
	char str[40];

	int player_count;
	
	int x, y;

	int item_w, spacing;

	player_count = (int)players->size();
	
	if (player_count == 0)
		return; // nothing to do then

	item_w = 160;

	x = 0;
	y = WINDOW_HEIGHT - 32;

	int divisor = player_count - 1;
	if (player_count == 1)
		divisor = 1;
	spacing = (WINDOW_WIDTH - (item_w * player_count)) / divisor;

	Player * player;

	for(unsigned int i = 0; i < players->size(); i++) {
		player = players->at(i);

		x = i * (item_w + spacing);

		// Avatar
		rect.x = x + 6;
		rect.y = y + 2;
		SDL_BlitSurface(player->sprites, main_.graphics->player_clip[SPR_R], screen, &rect);

		// Score
		sprintf_s(str, 40, "%02d", player->score);
		surface = main_.text->render_text_medium(str);
		rect.x = x + 32;
		rect.y = y + 14;
		SDL_BlitSurface(surface, NULL, screen, &rect);
		SDL_FreeSurface(surface);

		// Healthbar
		rect.x = x + 32;
		rect.y = y + 2;
		rect.w = 122;
		rect.h = 10;
		SDL_FillRect(screen, &rect, 0);

		rect_s.w = (int)(1.18 * player->hitpoints);
		rect_s.h = 8;
		rect_s.x = 0;
		rect_s.y = 0;
		rect.x = x + 34;
		rect.y = y + 4;
		SDL_BlitSurface(main_.graphics->player1hp, &rect_s, screen, &rect);

		// Bullets
		rect_s.x = 0;
		rect_s.y = 0;
		rect_s.w = 8;
		rect_s.h = 8;
		rect.x = x + 118;
		rect.y = y + 18;
		if(player->doubledamagebullets == -1 || player->doubledamagebullets > 0) rect_s.x = 8;
		if(player->instantkillbullets == -1 || player->instantkillbullets > 0) rect_s.x = 16;
		SDL_BlitSurface(main_.graphics->weapons, &rect_s, screen, &rect);

		if(player->instantkillbullets > 0) ammount = player->instantkillbullets;
		else if(player->doubledamagebullets > 0) ammount = player->doubledamagebullets;
		else {
			ammount = player->bullets;
		}

		if(ammount != -1) {
			sprintf_s(str, 3, "%02d", ammount);
			surface = main_.text->render_text_medium_gray(str);
			rect.x = x + 132;
			rect.y = y + 14;
			SDL_BlitSurface(surface, NULL, screen, &rect);
			SDL_FreeSurface(surface);
		} else {
			rect_s.x = 0;
			rect_s.y = 0;
			rect_s.w = 16;
			rect_s.h = 16;
			rect.x = x + 132;
			rect.y = y + 14;
			SDL_BlitSurface(main_.graphics->common, &rect_s, screen, &rect);
		}

		// Bombs
		if(player->mines > 0 || player->mines == -1) {
			rect_s.x = 54;
			rect_s.y = 0;
			rect_s.w = 6;
			rect_s.h = 4;
			rect.x = x + 70;
			rect.y = y + 24;
		} else {
			rect_s.x = 12;
			rect_s.y = 0;
			rect_s.w = 12;
			rect_s.h = 16;
			rect.x = x + 66;
			rect.y = y + 14;
		}
		SDL_BlitSurface(main_.graphics->bombs, &rect_s, screen, &rect);

		if(player->bombs != -1 || player->mines != -1) {
			if(player->mines > 0)
				sprintf_s(str, 3, "%02d", player->mines);
			else
				sprintf_s(str, 3, "%02d", player->bombs);
			surface = main_.text->render_text_medium_gray(str);
			rect.x = x + 84;
			rect.y = y + 14;
			SDL_BlitSurface(surface, NULL, screen, &rect);
			SDL_FreeSurface(surface);
		} else {
			rect_s.x = 0;
			rect_s.y = 0;
			rect_s.w = 16;
			rect_s.h = 16;
			rect.x = x + 84;
			rect.y = y + 14;
			SDL_BlitSurface(main_.graphics->common, &rect_s, screen, &rect);
		}
	}
}

void LocalMultiplayer::draw_game_ended() {
	if (main_.no_sdl)
		return;
	SDL_Surface * surface;
	SDL_Rect rect;
	SDL_Rect clip_avatar;

	char text[30];
	
	if (!winner)
		return;

	if(draw)
		sprintf(text, "DRAW");
	else
		sprintf(text, "%s WINS", winner->name);

	surface = main_.text->render_text_medium_shadow(text);
	rect.x = (screen->w - surface->w) / 2;
	if(!draw) rect.x += 28;
	rect.y = (screen->h - surface->h) / 2;

	SDL_BlitSurface(surface, NULL, screen, &rect);
	SDL_FreeSurface(surface);
	
	if(!draw) {
		rect.x = rect.x - 56;
		rect.y = (screen->h - 44) / 2;
		clip_avatar.x = 220;
		clip_avatar.y = 0;
		clip_avatar.w = 44;
		clip_avatar.h = 44;
		if((frame - end_avatar_start_frame) % 40 > 20)
			clip_avatar.y = 44;

		SDL_BlitSurface(winner->sprites, &clip_avatar, screen, &rect);
	}
}

void LocalMultiplayer::on_pre_delete_player(const Player &player)
{
	if (winner && winner->number == player.number)
		winner = NULL;
}