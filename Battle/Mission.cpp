#include "SDL/SDL.h"

#include "Main.h"
#include "Level.h"
#include "Gameplay.h"

#include "HealthPowerUp.h"
#include "AmmoPowerUp.h"
#include "DoubleDamagePowerUp.h"
#include "InstantKillBulletPowerUp.h"
#include "BombPowerUp.h"
#include "AirstrikePowerUp.h"
#include "LaserBeamPowerUp.h"

#include "NPC.h"
#include "ChickNPC.h"
#include "CannonNPC.h"
#include "GatlingNPC.h"

#include "Mission.h"

#ifndef WIN32
#define sprintf_s snprintf
#endif

const int Mission::MISSION_COUNT = 9;
const MISSION_INFO Mission::MISSIONS[Mission::MISSION_COUNT] = {
	{(char*)"CHICK INVASION", (char*)"stage/chick_invasion.lvl"},
	{(char*)"CHICK INVASION II", (char*)"stage/chick_invasion_ii.lvl"},
	{(char*)"EASY DOES IT", (char*)"stage/easy_does_it.lvl"},
	{(char*)"CHICKEN RUN", (char*)"stage/chicken_run.lvl"},
	{(char*)"GUN TEST", (char*)"stage/guntest.lvl"},
	{(char*)"STOMP HIS HEAD", (char*)"stage/tryout.lvl"},
	{(char*)"SUCKERPUNCHER", (char*)"stage/tryout.lvl"},
	{(char*)"POKE THE EYE OUT", (char*)"stage/tryout.lvl"},
	{(char*)"KILL THE FAGGOT", (char*)"stage/tryout.lvl"},
};

Mission::Mission() {
	npcs_collide = false;

	mission_ended = false;

	player_won = false;
	cup = 2;
}

void Mission::initialize() {
	pause_menu = new PauseMenu(screen);
	pause_menu->add_option((char*)"RESUME\0");
	pause_menu->add_option((char*)"RESTART\0");
	pause_menu->add_option((char*)"QUIT\0");
}

void Mission::pause(Player *p) {
	int ret;
	ret = pause_menu->pause(p);
	if(ret == 1) {
		ended = false;
		mission_ended = false;
		reset_game();
	}
	if(ret == 2) game_running = false;
}

void Mission::on_game_reset() {
	if(mission_ended) {
		game_running = false;
		return;
	}

	Player * p;
	unsigned int i;
	LEVEL_POWERUP * lpu;
	GameplayObject * gpo;
	LEVEL_NPC * lnpc;
	NPC * npc;
	SDL_Rect * rect, * pos;

	time = 0;

	p = players->at(0);
	p->reset();

	p->set_character(level->mission.character);

	p->position->x = level->playerstart[0].x * TILE_W + ((TILE_W - PLAYER_W) / 2);
	p->position->y = level->playerstart[0].y * TILE_H - PLAYER_H;
	p->set_sprite(level->playerstart[0].facing_right ? SPR_R : SPR_L);

	p->bullets = level->mission.bullets;
	p->doubledamagebullets = level->mission.doubledamagebullets;
	p->instantkillbullets = level->mission.instantkillbullets;
	p->bombs = level->mission.bombs;

	// Reset Powerups
	for(i = 0; i < level->powerups->size(); i++) {
		gpo = NULL;
		lpu = level->powerups->at(i);
		rect = new SDL_Rect();
		pos = new SDL_Rect();
		pos->x = lpu->position.x; pos->y = lpu->position.y;
		pos->w = 16; pos->h = 16;
		rect->w = 16; rect->h = 16;
		switch(lpu->type) {
			case L_PU_HEALTH:
				rect->x = 0; rect->y = 0;
				gpo = new HealthPowerUp(Main::graphics->powerups, rect, pos, 25);
				break;
			case L_PU_AMMO:
				rect->x = 32; rect->y = 0;
				gpo = new AmmoPowerUp(Main::graphics->powerups, rect, pos, 20);
				break;
			case L_PU_DOUBLEDAMAGE:
				rect->x = 48; rect->y = 0;
				gpo = new DoubleDamagePowerUp(Main::graphics->powerups, rect, pos, 5);
				break;
			case L_PU_INSTANTKILL:
				rect->x = 64; rect->y = 0;
				gpo = new InstantKillBulletPowerUp(Main::graphics->powerups, rect, pos, 1);
				break;
			case L_PU_BOMB:
				rect->x = 16; rect->y = 0;
				gpo = new BombPowerUp(Main::graphics->powerups, rect, pos, 1);
				break;
			case L_PU_AIRSTRIKE:
				rect->x = 80; rect->y = 0;
				gpo = new AirstrikePowerUp(Main::graphics->powerups, rect, pos);
				break;
			case L_PU_LASERBEAM:
				rect->x = 112; rect->y = 0;
				gpo = new LaserBeamPowerUp(Main::graphics->powerups, rect, pos);
				break;
		}
		if(gpo == NULL) {
			delete rect;
			delete pos;
			continue;
		}

		add_object(gpo);
	}

	// Delete old NPCs
	for(i = 0; i < npcs->size(); i++) {
		delete npcs->at(i);
	}
	npcs->clear();

	// Reset NPCs
	for(i = 0; i < level->npcs->size(); i++) {
		npc = NULL;
		lnpc = level->npcs->at(i);
		switch(lnpc->type) {
			case L_NPC_CHICK:
				npc = new ChickNPC();
				break;
			case L_NPC_CANNON:
				npc = new CannonNPC();
				break;
			case L_NPC_GATLING:
				npc = new GatlingNPC();
				break;
		}
		if(npc == NULL) continue;

		npc->move_direction = lnpc->move_direction;		
		npc->position->x = lnpc->position.x;
		npc->position->y = lnpc->position.y;

		npc->reset();

		add_npc(npc);
	}

	time = 0;
}

void Mission::on_pre_processing() {
	if(!countdown && !ended) {
		time++;
	}
}

void Mission::on_post_processing() {
	Player * p;
	p = players->at(0);

	if(!countdown && !ended) {
		if(p->hitpoints < 0)
			p->hitpoints = 0;

		if(!p->is_dead) {
			if(p->hitpoints == 0) {
				Main::audio->play(SND_YOULOSE, p->position->x);

				p->is_dead = true;
				p->dead_start = Gameplay::frame;
				p->is_hit = true;

				ended = true;
				end_start = frame;
				mission_ended = true;
				player_won = false;
			}
		}
	}

	if(!countdown && !ended) {
		if(level->mission.type == LM_TYPE_KILL_ALL) {
			int alive;
			NPC * npc;
			alive = 0;

			for(unsigned int i = 0; i < npcs->size(); i++) {
				npc = npcs->at(i);
				if(!npc->is_stationary && !npc->is_dead)
					alive++;
			}
			if(alive == 0) {
				mission_ended = true;
				ended = true;
				end_start = frame;
				player_won = true;
				cup = 2;
				if(time <= level->mission.kill_all_time_silver)
					cup = 1;
				if(time <= level->mission.kill_all_time_gold)
					cup = 0;
			}
		}
	}
}

void Mission::draw_score() {
	Player * p;
	SDL_Surface * surface;
	SDL_Rect rect;
	SDL_Rect rect_s;
	int ammount;
	char str[40];

	// Fill the background
	rect.x = 0;
	rect.y = WINDOW_HEIGHT - 32;
	rect.w = WINDOW_WIDTH;
	rect.h = 32;
	SDL_FillRect(screen, &rect, 0x222222);

	// Show the time
	int min, sec, milsec;

	min = time / 3600;
	sec = time / 60 % 60;
	milsec = (int)((time % 60) * 1.6667);

	sprintf_s(str, 10, "%02d:%02d", min, sec);
	surface = Main::text->render_text_large(str);
	rect.x = 480;
	rect.y = 450;
	SDL_BlitSurface(surface, NULL, screen, &rect);
	SDL_FreeSurface(surface);
	
	sprintf_s(str, 10, ".%02d", milsec);
	surface = Main::text->render_text_medium(str);
	rect.x = 602;
	rect.y = 464;
	SDL_BlitSurface(surface, NULL, screen, &rect);
	SDL_FreeSurface(surface);

	// Draw the right cup
	if(level->mission.type == LM_TYPE_KILL_ALL) {
		int cup, target;

		target = -1;
		cup = 2;
		if(time <= level->mission.kill_all_time_silver) {
			target = level->mission.kill_all_time_silver;
			cup = 1;
		}
		if(time <= level->mission.kill_all_time_gold) {
			target = level->mission.kill_all_time_gold;
			cup = 0;
		}

		// The cup will flicker if cup is about to change within 128 frames
		if(target == -1 || ((target - time > 128) || ((target - time) & 0x10) != 0x10)) {
			rect_s.x = CUP_W * cup;
			rect_s.y = 0;
			rect_s.w = CUP_W;
			rect_s.h = CUP_H;
			rect.x = 440;
			rect.y = 450;

			SDL_BlitSurface(Main::graphics->cups, &rect_s, screen, &rect);
		}
	}

	p = players->at(0);

	// Show player avatars
	rect.x = 2;
	rect.y = 450;
	SDL_BlitSurface(p->sprites, Main::graphics->player_clip[SPR_R], screen, &rect);
	
	// Health bar
	rect.x = 30;
	rect.y = WINDOW_HEIGHT - 30;
	rect.w = 122;
	rect.h = 10;
	SDL_FillRect(screen, &rect, 0);

	rect_s.w = (int)(1.18 * p->hitpoints);
	rect_s.h = 8;
	rect_s.x = 0;
	rect_s.y = 0;
	rect.x = 32;
	rect.y = WINDOW_HEIGHT - 28;
	SDL_BlitSurface(Main::instance->graphics->player1hp, &rect_s, screen, &rect);
	
	// Player name
	surface = Main::graphics->playername->at(p->character);
	rect.x = 240 - surface->w;
	rect.y = 455;
	SDL_BlitSurface(surface, NULL, screen, &rect);
	
	// Show bomb ammount
	rect_s.x = 12;
	rect_s.y = 0;
	rect_s.w = 12;
	rect_s.h = 16;
	rect.x = 30;
	rect.y = 462;
	SDL_BlitSurface(Main::graphics->bombs, &rect_s, screen, &rect);

	rect_s.x = 0;
	rect_s.y = 0;
	rect_s.w = 16;
	rect_s.h = 16;

	if(p->bombs != -1) {
		sprintf_s(str, 3, "%02d", p->bombs);
		surface = Main::text->render_text_medium(str);
		rect.x = 46;
		rect.y = 464;
		SDL_BlitSurface(surface, NULL, screen, &rect);
		SDL_FreeSurface(surface);
	} else {
		rect.x = 46;
		rect.y = 462;
		SDL_BlitSurface(Main::graphics->common, &rect_s, screen, &rect);
	}
	
	// Ammo type and ammount
	rect_s.x = 0;
	rect_s.y = 0;
	rect_s.w = 8;
	rect_s.h = 8;
	rect.x = 90;
	rect.y = 466;
	if(p->doubledamagebullets > 0) rect_s.x = 8;
	if(p->instantkillbullets > 0) rect_s.x = 16;
	SDL_BlitSurface(Main::graphics->weapons, &rect_s, screen, &rect);

	if(p->instantkillbullets > 0) ammount = p->instantkillbullets;
	else if(p->doubledamagebullets > 0) ammount = p->doubledamagebullets;
	else ammount = p->bullets;

	if(ammount != -1) {
		sprintf_s(str, 3, "%02d", ammount);
		surface = Main::text->render_text_medium(str);
		rect.x = 102;
		rect.y = 464;
		SDL_BlitSurface(surface, NULL, screen, &rect);
		SDL_FreeSurface(surface);
	} else {
		rect_s.x = 0;
		rect_s.y = 0;
		rect_s.w = 16;
		rect_s.h = 16;
		rect.x = 102;
		rect.y = 462;
		SDL_BlitSurface(Main::graphics->common, &rect_s, screen, &rect);
	}
}

void Mission::draw_game_ended() {
	SDL_Surface * surface;
	SDL_Rect rect, rect_s;

	char text[30];
	
	if(player_won)
		sprintf(text, "LEVEL CLEARED!");
	else
		sprintf(text, "YOU LOST");

	surface = Main::text->render_text_medium(text);
	rect.x = (screen->w - surface->w) / 2;
	if(player_won)
		rect.x += CUP_W / 2 + 4;
	rect.y = (screen->h - surface->h) / 2;

	SDL_BlitSurface(surface, NULL, screen, &rect);
	SDL_FreeSurface(surface);

	if(player_won && ((frame - end_start) & 0x10) == 0x10) {
		rect_s.x = CUP_W * cup;
		rect_s.y = 0;
		rect_s.w = CUP_W;
		rect_s.h = CUP_H;
		rect.x = rect.x - CUP_W - 8;
		rect.y = (screen->h - CUP_H) / 2; 

		SDL_BlitSurface(Main::graphics->cups, &rect_s, screen, &rect);
	}
}
