#pragma once

#include <iostream>

#include "Timer.h"
#include "Text.h"
#include "Graphics.h"
#include "AudioController.h"
#include "GameInput.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define MENU_CURSOR_COLOR 0x0088ff

#define SAVE_SIGNATURE 0x5353 // 'SS'
#define SAVE_VERSION 2

struct SaveHeader {
	short signature;
	short version;
};

enum class MainRunModes
{ 
	ARCADE,
	SERVER,
	CLIENT
};

class Gameplay;
namespace network {
class ServerClient;
class Server;
}

class Main {
public:

	bool no_sdl;

	//static Main * instance;

	SDL_Surface * screen;
	int flags;

	bool running;
	int frame_delay;
	unsigned int frame;
	bool fps_cap;

	bool screenshot_next_flip;

	const int FRAMES_PER_SECOND;
	const int MILLISECS_PER_FRAME;

	const int FRAMES_UNTIL_RESET;

	const int CONTROLS_REPEAT_DELAY;
	const int CONTROLS_REPEAT_SPEED;

	const int JOYSTICK_AXIS_THRESHOLD;

	MainRunModes runmode;

	Timer * fps;

	int fps_counter_last_frame;
	int fps_counter_this_frame;
	Timer * fps_counter_timer;
	bool fps_counter_visible;
	bool ingame_debug_visible;

	AudioController * audio;
	Graphics * graphics;
	Text * text;

	Gameplay *gameplay_;

	unsigned int last_activity;
	bool autoreset;
	bool is_reset;

	SDL_Joystick * joystick[10];

	GameInput * input[4];
	GameInput * input_master;

	Uint8 online_character;

	Main();
	~Main();

	network::ServerClient &getServerClient();
	network::Server &getServer();

	void setGameplay(Gameplay *gameplay);
	Gameplay &gameplay();

	int run(const MainRunModes &);
	void flip(bool no_cap = false);
	void handle_event(SDL_Event * event);
	void reset_inputs();
private:
	bool init();
	void clean_up();

	void fps_count();

	void take_screenshot();

	void load_options();
	void load_default_options();
	void save_options();

	void set_default_controlschemes();

	network::ServerClient *serverClient_;
	network::Server *server_;
};
