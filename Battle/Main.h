#ifndef _MAIN_H
#define _MAIN_H

#include "SDL/SDL_ttf.h"

#include "Timer.h"
#include "AudioController.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

struct ControlScheme {
	bool use_keyboard;
	int kb_left;
	int kb_right;
	int kb_up;
	int kb_down;
	int kb_jump;
	int kb_run;
	int kb_shoot;
	int kb_bomb;
	int kb_start;

	bool use_joystick;
	int joystick_idx;
	bool use_axis_x;
	bool use_axis_up;
	bool use_axis_down;
	int js_left;
	int js_right;
	int js_jump;
	int js_down;
	int js_run;
	int js_shoot;
	int js_bomb;
	int js_start;
};

class Main {
public:
	static Main * instance;

	static SDL_Surface * screen;
	static int flags;

	static TTF_Font * font;

	static bool running;
	static int frame_delay;
	static int frame;
	static bool fps_cap;

	static const int FRAMES_PER_SECOND;

	static const int CONTROLS_REPEAT_DELAY;
	static const int CONTROLS_REPEAT_SPEED;

	static const int JOYSTICK_AXIS_THRESHOLD;

	static Timer * fps;

	static AudioController * audio;

	SDL_Joystick * joystick1;
	SDL_Joystick * joystick2;

	ControlScheme controls1;
	ControlScheme controls2;

	Main();
	~Main();
	int run();
	void flip();
	void handle_event(SDL_Event * event);
private:
	bool init();
	void clean_up();

	void load_options();
	void save_options();

	void set_default_controlschemes();
};

#endif

