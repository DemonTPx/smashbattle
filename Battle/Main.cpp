#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"

#include <cstdio>
#include <iostream>
#include <fstream>

#include "Timer.h"
#include "Menu.h"
#include "AudioController.h"
#include "Graphics.h"

#include "Main.h"

const int Main::FRAMES_PER_SECOND = 60;

const int Main::CONTROLS_REPEAT_DELAY = 30;
const int Main::CONTROLS_REPEAT_SPEED = 10;

const int Main::JOYSTICK_AXIS_THRESHOLD = 0x3fff;

Main * Main::instance = NULL;

SDL_Surface * Main::screen = NULL;
int Main::flags = SDL_SWSURFACE;

bool Main::running = false;
int Main::frame_delay = 0;
int Main::frame = 0;
bool Main::fps_cap = false;

Timer * Main::fps = NULL;

int Main::fps_counter_last_frame = 0;
int Main::fps_counter_this_frame = 0;
Timer * Main::fps_counter_timer = NULL;
bool Main::fps_counter_visible = false;

AudioController * Main::audio = NULL;
Graphics * Main::graphics = NULL;
Text * Main::text = NULL;

Main::Main() {
	Main::instance = this;
}

Main::~Main() {
	if(Main::instance == this) {
		Main::instance = NULL;
	}
}

bool Main::init() {
	//Start SDL
	SDL_Init(SDL_INIT_EVERYTHING);
	
	screen = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32, flags);
	SDL_ShowCursor(0);

	fps_cap = true;

	if(screen == NULL) return false;

	SDL_WM_SetCaption("Battle", NULL);
	
	fps = new Timer();

	fps_counter_timer = new Timer();

	audio = new AudioController();
	audio->open_audio();
	audio->load_files();

	graphics = new Graphics();
	graphics->load_all();

	text = new Text();
	text->load_all();

	// enable joystick throughout the game
	SDL_JoystickEventState(SDL_ENABLE);
	for(int i = 0; i < SDL_NumJoysticks(); i++) {
		joystick[i] = SDL_JoystickOpen(i);
	}

	return true;
}

void Main::clean_up() {
	SDL_FreeSurface(screen);

	delete fps;

	delete fps_counter_timer;

	audio->close_files();
	audio->close_audio();
	delete audio;

	graphics->clear_all();
	delete graphics;

	text->clear_all();
	delete text;

	for(int i = 0; i < 10; i++) {
		if(SDL_JoystickOpened(i)) {
			SDL_JoystickClose(joystick[i]);;
		}
	}

	//Quit SDL
	SDL_Quit();
}

void Main::flip() {
	fps_count();

	SDL_Flip(screen);
	frame++;
	if((fps_cap == true) && (fps->get_ticks() < frame_delay)) {
		SDL_Delay((frame_delay) - fps->get_ticks());
	}

	fps->start();
}

void Main::fps_count() {
	// Calculate the FPS
	if(fps_counter_timer->get_ticks() > 1000) {
		fps_counter_this_frame = frame - fps_counter_last_frame;

		fps_counter_last_frame = frame;
		fps_counter_timer->start();
	}

	// Show FPS
	if(fps_counter_visible) {
		char * cap;
		cap = new char[20];

		SDL_Surface * surf;
		SDL_Rect rect;
		SDL_Color color;
		
		color.r = 0xff;
		color.g = 0xff;
		color.b = 0xff;

		sprintf(cap, "%d FPS", fps_counter_this_frame);
		surf = Main::text->render_text_small(cap);

		rect.x = screen->w - surf->w - 2;
		rect.y = 2;

		SDL_BlitSurface(surf, NULL, screen, &rect);

		SDL_FreeSurface(surf);
		delete cap;
	}
}

void Main::handle_event(SDL_Event * event) {
	/* Catch quit event and ALT-F4 */
	if(event->type == SDL_QUIT) {
		running = false;
	}
	if(event->type == SDL_KEYDOWN) {
		if(event->key.keysym.mod & KMOD_ALT) {
			if(event->key.keysym.sym == SDLK_F4) {
				running = false;
			}
		}
		if(event->key.keysym.sym == SDLK_F10) {
			SDL_WM_ToggleFullScreen(screen);
		}
		if(event->key.keysym.sym == SDLK_F11) {
			fps_counter_visible = !fps_counter_visible;
		}
	}
}

int Main::run() {
	if(!init()) return 1;

	frame_delay = 1000 / FRAMES_PER_SECOND;
	frame = 0;

	load_options();

	fps_counter_timer->start();

	Menu * menu;
	menu = new Menu();

	running = true;
	menu->run();

	delete menu;

	save_options();

	clean_up();

#ifdef _DEBUG
#ifdef WIN32
	_CrtDumpMemoryLeaks();
#endif
#endif
	return 0;
}


int main(int argc, char* args[]) {
	if(argc > 1) {
		if(strcmp(args[1], "-f") == 0) {
			Main::flags |= SDL_FULLSCREEN;
		}
	}
	Main main;
	return main.run();
}

void Main::load_options() {
	std::ifstream file;
	ControlScheme * controls;

	file.open("options.dat", std::ifstream::in | std::ifstream::binary);

	if(file.eof() || file.fail()) {
		audio->sound_volume = 100;
		audio->music_volume = 100;
		set_default_controlschemes();
		file.close();
		return;
	}

	file.read((char*)&audio->sound_volume, sizeof(int));
	file.read((char*)&audio->music_volume, sizeof(int));

	for(int i = 0; i < 4; i++) {
		if(i == 0) controls = &controls1;
		if(i == 1) controls = &controls2;
		if(i == 2) controls = &controls3;
		if(i == 3) controls = &controls4;

		file.read((char*)controls, sizeof(ControlScheme));

		if(file.eof()) break;
	}
	file.close();
}

void Main::save_options() {
	std::ofstream file;
	ControlScheme * controls;

	file.open("options.dat", std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);

	if(file.fail()) {
		file.close();
	}

	file.write((char*)&audio->sound_volume, 4);
	file.write((char*)&audio->music_volume, 4);
	

	for(int i = 0; i < 4; i++) {
		if(i == 0) controls = &controls1;
		if(i == 1) controls = &controls2;
		if(i == 2) controls = &controls3;
		if(i == 3) controls = &controls4;

		file.write((char*)controls, sizeof(ControlScheme));
	}

	file.close();
}

void Main::set_default_controlschemes() {
	controls1.use_keyboard = true;
	controls1.kb_left = SDLK_a;
	controls1.kb_right = SDLK_d;
	controls1.kb_up = SDLK_w;
	controls1.kb_down = SDLK_s;
	controls1.kb_jump = SDLK_w;
	controls1.kb_run = SDLK_LSHIFT;
	controls1.kb_shoot = SDLK_LCTRL;
	controls1.kb_bomb = SDLK_LALT;
	controls1.kb_start = SDLK_ESCAPE;
	controls1.use_joystick = true;
	controls1.use_axis_x = true;
	controls1.use_axis_up = false;
	controls1.use_axis_down = true;
	controls1.joystick_idx = 0;
	controls1.js_jump = 2;
	controls1.js_run = 3;
	controls1.js_shoot = 5;
	controls1.js_bomb = 7;
	controls1.js_start = 9;

	controls2.use_keyboard = true;
	controls2.kb_left = SDLK_LEFT;
	controls2.kb_right = SDLK_RIGHT;
	controls2.kb_up = SDLK_UP;
	controls2.kb_down = SDLK_DOWN;
	controls2.kb_jump = SDLK_UP;
	controls2.kb_run = SDLK_RSHIFT;
	controls2.kb_shoot = SDLK_RCTRL;
	controls2.kb_bomb = SDLK_RALT;
	controls2.kb_start = SDLK_RETURN;
	controls2.use_joystick = true;
	controls2.use_axis_x = true;
	controls2.use_axis_up = false;
	controls2.use_axis_down = true;
	controls2.joystick_idx = 1;
	controls2.js_jump = 2;
	controls2.js_run = 3;
	controls2.js_shoot = 5;
	controls2.js_bomb = 7;
	controls2.js_start = 9;
}
