#ifndef _MAIN_H
#define _MAIN_H

#include <iostream>

#include "Timer.h"
#include "Text.h"
#include "Graphics.h"
#include "AudioController.h"
#include "GameInput.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define SAVE_SIGNATURE 0x5353 // 'SS'
#define SAVE_VERSION 1

struct SaveHeader {
	short signature;
	short version;
};

class Main {
public:
	static Main * instance;

	static SDL_Surface * screen;
	static int flags;

	static bool running;
	static int frame_delay;
	static unsigned int frame;
	static bool fps_cap;

	static bool screenshot_next_flip;

	static const int FRAMES_PER_SECOND;

	static const int FRAMES_UNTIL_RESET;

	static const int CONTROLS_REPEAT_DELAY;
	static const int CONTROLS_REPEAT_SPEED;

	static const int JOYSTICK_AXIS_THRESHOLD;

	static Timer * fps;

	static int fps_counter_last_frame;
	static int fps_counter_this_frame;
	static Timer * fps_counter_timer;
	static bool fps_counter_visible;

	static AudioController * audio;
	static Graphics * graphics;
	static Text * text;

	static unsigned int last_activity;
	static bool autoreset;
	static bool is_reset;

	SDL_Joystick * joystick[10];

	GameInput * input[4];
	GameInput * input_master;

	Main();
	~Main();
	int run();
	void flip();
	void handle_event(SDL_Event * event);
private:
	bool init();
	void clean_up();

	void fps_count();

	void take_screenshot();

	void load_options();
	void save_options();

	void set_default_controlschemes();
};

#endif

