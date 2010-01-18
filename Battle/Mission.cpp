#include "SDL/SDL.h"

#include "Main.h"
#include "Level.h"
#include "Gameplay.h"

#include "NPC.h"
#include "ChickNPC.h"

#include "Mission.h"

#ifndef WIN32
#define sprintf_s snprintf
#endif

const int Mission::MISSION_COUNT = 9;
const MISSION_INFO Mission::MISSIONS[Mission::MISSION_COUNT] = {
	{(char*)"INVASION OF THE CHICKS", (char*)"mission/001.lvl"},
	{(char*)"CHOP THE OFFICER", (char*)"mission/002.lvl"},
	{(char*)"KICK SOME ASS", (char*)"mission/003.lvl"},
	{(char*)"PUSH 'EM IN THE PIT", (char*)"mission/004.lvl"},
	{(char*)"BURN THEM WITH A JALAPENO", (char*)"mission/005.lvl"},
	{(char*)"STOMP HIS HEAD", (char*)"mission/006.lvl"},
	{(char*)"SUCKERPUNCHER", (char*)"mission/007.lvl"},
	{(char*)"POKE THE EYE OUT", (char*)"mission/008.lvl"},
	{(char*)"KILL THE FAGGOT", (char*)"mission/009.lvl"},
};

Mission::Mission() {
	bullets = -1;
	bombs = 3;
}

void Mission::initialize() {
	Gameplay::initialize();
}

void Mission::on_game_reset() {
	Player * p;
	p = players->at(0);
	p->reset();
	p->bullets = bullets;
	p->bombs = bombs;

	time = 0;
}

void Mission::on_pre_processing() {
	if(!countdown) {
		time++;
	}
}

void Mission::on_post_processing() {
	Player * p;
	p = players->at(0);

	if(!countdown || !ended) {
		if(p->hitpoints < 0)
			p->hitpoints = 0;

		if(!p->is_dead) {
			if(p->hitpoints == 0) {
				Main::audio->play(SND_YOULOSE, p->position->x);

				p->is_dead = true;
				p->dead_start = Gameplay::frame;
				p->is_hit = true;
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
