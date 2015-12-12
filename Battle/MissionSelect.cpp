#include "SDL/SDL.h"

#include "Main.h"
#include "Player.h"
#include "Color.h"

#include <vector>

#include "Mission.h"

#include "MissionSelect.h"

#ifndef WIN32
#define sprintf_s snprintf
#endif

#define DIRECTION_NONE	0
#define DIRECTION_LEFT	1
#define DIRECTION_RIGHT	2
#define DIRECTION_UP	4
#define DIRECTION_DOWN	8

#define MISSIONSELECT_HEIGHT 50
#define MISSIONSELECT_COUNT 6

MissionSelect::MissionSelect(Main &main) : SimpleDrawable(main), main_(main) {
	cancel = false;

	mission = 0;
	mission_scroll_top = 0;
}

void MissionSelect::run() {
	SDL_Event event;

	load_sprites();

	input = main_.input_master;
	input->reset();
	input->set_delay();

	ready = false;
	ready_mission = false;
	cancel = false;
	cancel_selected = false;
	
	select(DIRECTION_NONE);

	frame = 0;

	while (main_.running && !ready) {
		while(SDL_PollEvent(&event)) {
			main_.handle_event(&event);
			
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
				ready = true;
				cancel = true;
				break;
			}

			input->handle_event(&event);
		}

		process_cursor();

		frame++;

		draw();

		if(ready_mission && flicker_frame > 0x20) {
			ready = true;
		}

		main_.flip();
	}

	if(!ready)
		cancel = true;

	free_sprites();
}

void MissionSelect::process_cursor() {
	int direction;

	if (input->is_pressed(A_BACK)) {
		ready = true;
		cancel = true;
		return;
	}

	if(input->is_pressed(A_RUN) || input->is_pressed(A_JUMP) ||
		input->is_pressed(A_SHOOT) || input->is_pressed(A_BOMB)) {
			if(!(input->is_pressed(A_JUMP) && input->is_pressed(A_UP))) { // It's likely that up and jump are the same keybind
				if(cancel_selected) {
					ready = true;
					cancel = true;
				}
				if(!ready_mission) {
					ready_mission = true;
					
					main_.audio->play(SND_SELECT_CHARACTER);

					flicker = true;
					flicker_frame = 0;
				}
			}
	}
	
	direction = 0;
	if(input->is_pressed(A_LEFT)) direction |= DIRECTION_LEFT;
	if(input->is_pressed(A_RIGHT)) direction |= DIRECTION_RIGHT;
	if(input->is_pressed(A_UP)) direction |= DIRECTION_UP;
	if(input->is_pressed(A_DOWN)) direction |= DIRECTION_DOWN;
	if(direction != DIRECTION_NONE) {
		if(!ready_mission) {
			select(direction);
			main_.audio->play(SND_SELECT);
		}
	}
}

void MissionSelect::select(int direction) {
	if(direction & DIRECTION_LEFT) {
	}
	if(direction & DIRECTION_RIGHT) {
	}
	if(direction & DIRECTION_UP) {
		if(cancel_selected) {
			mission = Mission::MISSION_COUNT - 1;
			mission_scroll_top = Mission::MISSION_COUNT - MISSIONSELECT_COUNT;
			cancel_selected = false;
		} else if(mission == 0) {
			cancel_selected = true;
		} else {
			mission--;
		}
		if(mission_scroll_top != 0 && mission == mission_scroll_top)
			mission_scroll_top--;
	}
	if(direction & DIRECTION_DOWN) {
		if(cancel_selected) {
			mission = 0;
			mission_scroll_top = 0;
			cancel_selected = false;
		} else if(mission == (Mission::MISSION_COUNT - 1)) {
			cancel_selected = true;
		} else {
			mission++;
		}
		if(mission_scroll_top != Mission::MISSION_COUNT - MISSIONSELECT_COUNT && mission == mission_scroll_top + MISSIONSELECT_COUNT - 1)
			mission_scroll_top++;
	}
}

void MissionSelect::draw_impl() {
	SDL_Surface * screen;
	SDL_Surface * surface;
	SDL_Rect r_block, rect, rect_b, rect_s;
	Uint32 color;
	char text[4];

	screen = main_.screen;

	SDL_BlitSurface(background, NULL, screen, NULL);

	// Levels
	for(int idx = mission_scroll_top; idx < mission_scroll_top + MISSIONSELECT_COUNT && idx < Mission::MISSION_COUNT; idx++) {
		// Border
		r_block.x = 20;
		r_block.y = 20 + ((idx - mission_scroll_top) * 60);
		r_block.w = WINDOW_WIDTH - 40;
		r_block.h = MISSIONSELECT_HEIGHT;

		color = 0x888888;

		SDL_FillRectColor(screen, &r_block, color);

		// Highlight
		rect.x = r_block.x + 2;
		rect.y = r_block.y + 2;
		rect.w = r_block.w - 4;
		rect.h = r_block.h - 4;

		color = 0;
		if(!cancel_selected && mission == idx) {
			color = 0x0088ff;
			
			if(ready_mission && flicker) {
				if(flicker_frame > 0x20)
					flicker = false;
				if(flicker_frame & 0x4)
					color = 0xffffff;
				flicker_frame++;
			}
		}
		SDL_FillRectColor(screen, &rect, color);

		// Mission number
		sprintf_s(text, 4, "%02d", (idx + 1));
		rect.x = r_block.x + 60;
		rect.y = r_block.y + 10;
		surface = main_.text->render_text_large(text);
		SDL_BlitSurface(surface, 0, screen, &rect);
		SDL_FreeSurface(surface);

		// Mission name
		rect.x = r_block.x + 120;
		rect.y = r_block.y + 6;
		surface = main_.text->render_text_medium(Mission::MISSIONS[idx].name);
		SDL_BlitSurface(surface, 0, screen, &rect);
		SDL_FreeSurface(surface);

		// Time
		rect.x = r_block.x + 120;
		rect.y = r_block.y + 30;
		if(idx == 0)
			surface = main_.text->render_text_medium_gray("00:45:23");
		else
			surface = main_.text->render_text_medium_gray("--:--:--");
		SDL_BlitSurface(surface, 0, screen, &rect);
		SDL_FreeSurface(surface);

		// Cup
		int cup = idx % 4;
		if(cup < 3) {
			rect.x = r_block.x + r_block.w - CUP_W - 6;
			rect.y = r_block.y + 6;
			rect_s.x = (CUP_W * cup);
			rect_s.y = 0;
			rect_s.w = CUP_W;
			rect_s.h = CUP_H;
			SDL_BlitSurface(main_.graphics->cups, &rect_s, screen, &rect);
		}
	}

	surface = main_.text->render_text_medium((char*)"QUIT TO MENU\0");
	rect.x = WINDOW_WIDTH - 30 - surface->w;
	rect.y = WINDOW_HEIGHT - 30 - surface->h;
	if(cancel_selected) {
		rect_b.x = rect.x - 6;
		rect_b.y = rect.y - 6;
		rect_b.w = surface->w + 12;
		rect_b.h = surface->h + 12;
		SDL_FillRectColor(screen, &rect_b, 0x0088ff);
	}
	SDL_BlitSurface(surface, 0, screen, &rect);
	SDL_FreeSurface(surface);
}

void MissionSelect::load_sprites() {
	SDL_Rect rect;

	background = SDL_CreateRGBSurface(0, WINDOW_WIDTH, WINDOW_HEIGHT, 32, 0, 0, 0, 0);
	
	for(int y = 0; y < WINDOW_HEIGHT; y += main_.graphics->bg_grey->h) {
		for(int x = 0; x < WINDOW_WIDTH; x += main_.graphics->bg_grey->w) {
			rect.x = x;
			rect.y = y;
			SDL_BlitSurface(main_.graphics->bg_grey, NULL, background, &rect);
		}
	}
}

void MissionSelect::free_sprites() {
	SDL_FreeSurface(background);
}
