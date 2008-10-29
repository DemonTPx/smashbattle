#ifndef _MAIN_H
#define _MAIN_H

#include "SDL/SDL_ttf.h"

#include "Timer.h"
#include "AudioController.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

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

	static Timer * fps;

	static AudioController * audio;

	static bool music_on;
	static bool sound_on;

	SDL_Joystick * joystick1;
	SDL_Joystick * joystick2;

	Main();
	~Main();
	int run();
	void flip();
	void handle_event(SDL_Event * event);
	bool init();
	void clean_up();
};

#endif