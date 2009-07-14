#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"

#include <cstdio>
#include <iostream>
#include <fstream>

#include "Timer.h"
#include "Block.h"
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

	if(TTF_Init() == -1) return false;

	SDL_WM_SetCaption("Battle", NULL);
	
	fps = new Timer();

	fps_counter_timer = new Timer();

	audio = new AudioController();
	audio->open_audio();
	audio->load_files();

	graphics = new Graphics();
	graphics->load_all();

	// enable joystick throughout the game
	SDL_JoystickEventState(SDL_ENABLE);
	if(SDL_NumJoysticks() >= 1)
		joystick1 = SDL_JoystickOpen(0);
	if(SDL_NumJoysticks() >= 2)
		joystick2 = SDL_JoystickOpen(1);
	if(SDL_NumJoysticks() >= 3)
		joystick3 = SDL_JoystickOpen(2);
	if(SDL_NumJoysticks() >= 4)
		joystick4 = SDL_JoystickOpen(3);

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

	if(SDL_JoystickOpened(3))
		SDL_JoystickClose(joystick4);
	if(SDL_JoystickOpened(2))
		SDL_JoystickClose(joystick3);
	if(SDL_JoystickOpened(1))
		SDL_JoystickClose(joystick2);
	if(SDL_JoystickOpened(0))
		SDL_JoystickClose(joystick1);

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

		sprintf(cap, "%d fps", fps_counter_this_frame);
		surf = TTF_RenderText_Solid(graphics->font13, cap, color);

		rect.x = screen->w - surf->w;
		rect.y = 0;

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

#ifdef _DEBUG && WIN32
	_CrtDumpMemoryLeaks();
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
	union {
		int i;
		char c[4];
	} val;
	char c[1];
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

	file.read(val.c, 4);
	audio->sound_volume = val.i;
	file.read(val.c, 4);
	audio->music_volume = val.i;

	for(int i = 0; i < 4; i++) {
		if(i == 0) controls = &controls1;
		if(i == 1) controls = &controls2;
		if(i == 2) controls = &controls3;
		if(i == 3) controls = &controls4;

		file.read(c, 1);
		controls->use_keyboard = (c[0] == 1);
		file.read(val.c, 4);
		controls->kb_left = val.i;
		file.read(val.c, 4);
		controls->kb_right = val.i;
		file.read(val.c, 4);
		controls->kb_up = val.i;
		file.read(val.c, 4);
		controls->kb_down = val.i;
		file.read(val.c, 4);
		controls->kb_jump = val.i;
		file.read(val.c, 4);
		controls->kb_run = val.i;
		file.read(val.c, 4);
		controls->kb_shoot = val.i;
		file.read(val.c, 4);
		controls->kb_bomb = val.i;
		file.read(val.c, 4);
		controls->kb_start = val.i;
		file.read(c, 1);
		controls->use_joystick = (c[0] == 1);
		file.read(c, 1);
		controls->use_axis_x = (c[0] == 1);
		file.read(c, 1);
		controls->use_axis_up = (c[0] == 1);
		file.read(c, 1);
		controls->use_axis_down = (c[0] == 1);
		file.read(val.c, 4);
		controls->joystick_idx = val.i;
		file.read(val.c, 4);
		controls->js_jump = val.i;
		file.read(val.c, 4);
		controls->js_run = val.i;
		file.read(val.c, 4);
		controls->js_shoot = val.i;
		file.read(val.c, 4);
		controls->js_bomb = val.i;
		file.read(val.c, 4);
		controls->js_start = val.i;

		if(file.eof()) break;
	}
	file.close();
}

void Main::save_options() {
	union {
		int i;
		char c[4];
	} val;
	char c[1];
	std::ofstream file;
	ControlScheme * controls;

	file.open("options.dat", std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);

	if(file.fail()) {
		file.close();
	}

	val.i = audio->sound_volume;
	file.write(val.c, 4);
	val.i = audio->music_volume;
	file.write(val.c, 4);
	

	for(int i = 0; i < 4; i++) {
		if(i == 0) controls = &controls1;
		if(i == 1) controls = &controls2;
		if(i == 2) controls = &controls3;
		if(i == 3) controls = &controls4;

		c[0] = controls->use_keyboard ? 1 : 0;
		file.write(c, 1);
		val.i = controls->kb_left;
		file.write(val.c, 4);
		val.i = controls->kb_right;
		file.write(val.c, 4);
		val.i = controls->kb_up;
		file.write(val.c, 4);
		val.i = controls->kb_down;
		file.write(val.c, 4);
		val.i = controls->kb_jump;
		file.write(val.c, 4);
		val.i = controls->kb_run;
		file.write(val.c, 4);
		val.i = controls->kb_shoot;
		file.write(val.c, 4);
		val.i = controls->kb_bomb;
		file.write(val.c, 4);
		val.i = controls->kb_start;
		file.write(val.c, 4);
		c[0] = controls->use_joystick ? 1 : 0;
		file.write(c, 1);
		c[0] = controls->use_axis_x ? 1 : 0;
		file.write(c, 1);
		c[0] = controls->use_axis_up ? 1 : 0;
		file.write(c, 1);
		c[0] = controls->use_axis_down ? 1 : 0;
		file.write(c, 1);
		val.i = controls->joystick_idx;
		file.write(val.c, 4);
		val.i = controls->js_jump;
		file.write(val.c, 4);
		val.i = controls->js_run;
		file.write(val.c, 4);
		val.i = controls->js_shoot;
		file.write(val.c, 4);
		val.i = controls->js_bomb;
		file.write(val.c, 4);
		val.i = controls->js_start;
		file.write(val.c, 4);
	}

	file.close();
}

void Main::set_default_controlschemes() {
	//TODO: load these from a file
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
