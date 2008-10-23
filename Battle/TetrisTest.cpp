#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"

#include "Timer.h"
#include "Main.h"
#include "Block.h"

#include "TetrisTest.h"

TetrisTest::TetrisTest() {
	font28 = TTF_OpenFont("C:\\windows\\fonts\\tahoma.ttf", 28);
	fontColor.r = 255;
	fontColor.g = 255;
	fontColor.b = 255;

}

TetrisTest::~TetrisTest() {

}

void TetrisTest::run() {
	SDL_Event event;
	SDL_Surface * screen;
	Block block;
	Timer move_timer;
	int speed, speed_f;
	int y_speed, y_speed_n, y_speed_f;
	int frame_delay, frame_delay_n, frame_delay_f;
	int frame;
	bool fast;

	screen = Main::instance->screen;

	game_running = true;

	// Calculate the speed at wich the blocks fall
	fast = false;
	speed = 40; // speed = pixels per seconds
	speed_f = 400;

	if(speed >= (Main::FRAMES_PER_SECOND)) {
		frame_delay_n = 1;
		y_speed_n = speed / Main::FRAMES_PER_SECOND;
	} else {
		frame_delay_n = Main::FRAMES_PER_SECOND / speed;
		y_speed_n = 1;
	}

	if(speed_f >= (Main::FRAMES_PER_SECOND)) {
		frame_delay_f = 1;
		y_speed_f = speed_f / Main::FRAMES_PER_SECOND;
	} else {
		frame_delay_f = Main::FRAMES_PER_SECOND / speed_f;
		y_speed_f = 1;
	}

	y_speed = y_speed_n;
	frame_delay = frame_delay_n;


	block.x = (screen->w - block.w) / 2;
	block.y = 0;

	SDL_FillRect(screen, NULL, 0);

	frame = 0;

	while (Main::running && game_running) {
		while(SDL_PollEvent(&event)) {
			Main::instance->handle_event(&event);
			if(event.type == SDL_KEYDOWN) {
				if(event.key.keysym.sym == SDLK_ESCAPE) {
					game_running = false;
				}
				if(event.key.keysym.sym == SDLK_LEFT) {
					SDL_Rect * rect;
					rect = block.get_rect();
					SDL_FillRect(screen, rect, 0);
					delete rect;

					block.x -= block.w;
					block.show(screen);
				}
				if(event.key.keysym.sym == SDLK_RIGHT) {
					SDL_Rect * rect;
					rect = block.get_rect();
					SDL_FillRect(screen, rect, 0);
					delete rect;

					block.x += block.w;
					block.show(screen);
				}
				if(event.key.keysym.sym == SDLK_DOWN) {
					frame_delay = frame_delay_f;
					y_speed = y_speed_f;
				}
			}
			if(event.type == SDL_KEYUP) {
				if(event.key.keysym.sym == SDLK_DOWN) {
					frame_delay = frame_delay_n;
					y_speed = y_speed_n;
				}
			}
		}

		frame++;
		if(frame == frame_delay) {
			SDL_Rect * rect;
			rect = block.get_rect();
			SDL_FillRect(screen, rect, 0);
			delete rect;

			if(fast) {
				block.y += y_speed;
			} else {
				block.y += y_speed;
			}
			frame = 0;

			block.show(screen);
		}
		

		Main::instance->flip();
	}
}