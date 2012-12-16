#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_ttf.h"

#include <cstdio>
#include <iostream>
#include <fstream>
#include <ctime>
#include <algorithm>

#include "Timer.h"
#include "Menu.h"
#include "AudioController.h"
#include "Graphics.h"

#include "Util.h"

#include "Main.h"
#include "Server.h"
#include "ServerClient.h"

#include "states/ServerStates.h"
#include "log.h"


using std::string;
using std::vector;
using std::for_each;

#ifdef WIN32
#include <direct.h> // for _chdir
#else
#include <unistd.h> // for chdir
#endif

/**
 * Used in 'main' gameloop where frame_delay is no longer used.
 * A value of 17 would force the game to run in (1s = 1000ms / 17 =) ~58.82 fps
 */
const int Main::MILLISECS_PER_FRAME = 17;
/**
 * Used in all other places, like in menu, character set, these 'cap' to 60 fps.
 */
const int Main::FRAMES_PER_SECOND = 60;

const int Main::FRAMES_UNTIL_RESET = 7200;

const int Main::CONTROLS_REPEAT_DELAY = 30;
const int Main::CONTROLS_REPEAT_SPEED = 10;

const int Main::JOYSTICK_AXIS_THRESHOLD = 0x3fff;

Main * Main::instance = NULL;

SDL_Surface * Main::screen = NULL;
int Main::flags = SDL_SWSURFACE;

bool Main::running = false;
int Main::frame_delay = 0;
unsigned int Main::frame = 0;
bool Main::fps_cap = false;

bool Main::screenshot_next_flip = false;

Timer * Main::fps = NULL;

int Main::fps_counter_last_frame = 0;
int Main::fps_counter_this_frame = 0;
Timer * Main::fps_counter_timer = NULL;
bool Main::fps_counter_visible = false;

AudioController * Main::audio = NULL;
Graphics * Main::graphics = NULL;
Text * Main::text = NULL;

unsigned int Main::last_activity = 0;
bool Main::autoreset = true;
bool Main::is_reset = false;

Main::Main() : lag_(NULL)
{
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

	SDL_Surface * icon;
	Uint8 * mask;

	icon = Graphics::load_icon("gfx/SB.bmp", &mask, 0x00ffff);
	SDL_WM_SetIcon(icon, mask);

	SDL_FreeSurface(icon);
	delete[] mask;
	
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

	text = new Text();
	text->load_all();

	graphics = new Graphics();
	graphics->load_all();

	SDL_JoystickEventState(SDL_ENABLE);

	for(int i = 0; i < 4; i++) {
		input[i] = new GameInput();
	}
	input_master = NULL;

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

	for(int i = 0; i < 4; i++) {
		delete input[i];
	}

	//Quit SDL
	SDL_Quit();
}

// Todo refactor
#include "Commands.hpp"
void Main::flip(bool no_cap) {
	
	Server::getInstance().poll();

	ServerClient::getInstance().poll(0);

	if (ServerClient::getInstance().isConnected())
	{
		static Uint32 calculatedLag = SDL_GetTicks();
		Uint32 current = SDL_GetTicks();
		if (calculatedLag - current > 1000)
		{
			calculatedLag += 1000;
			CommandPing command;
			command.data.time = current;
			ServerClient::getInstance().send(command);
		}
	}

	fps_count();

	if(screenshot_next_flip) {
		take_screenshot();
		screenshot_next_flip = false;
	}

	SDL_Flip(screen);
	frame++;
	if(!no_cap && (fps_cap == true) && (fps->get_ticks() < frame_delay)) {
		SDL_Delay((frame_delay) - fps->get_ticks());
	}

	fps->start();

	if(autoreset) {
		if(frame - last_activity == (unsigned int)FRAMES_UNTIL_RESET) {
			running = false;
			is_reset = true;
		}
	}
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
		char cap[80];

		SDL_Surface * surf;
		SDL_Rect rect;
		SDL_Color color;
		
		color.r = 0xff;
		color.g = 0xff;
		color.b = 0xff;

		if (lag_)
			sprintf(cap, "%d FPS %f LAG", fps_counter_this_frame, lag_->avg());
		else
			sprintf(cap, "%d FPS", fps_counter_this_frame);

		surf = Main::text->render_text_small(cap);

		rect.x = screen->w - surf->w - 2;
		rect.y = 2;

		SDL_BlitSurface(surf, NULL, screen, &rect);

		SDL_FreeSurface(surf);
	}
}

void Main::take_screenshot() {
	time_t rawtime;
	struct tm * timeinfo;
	char filename[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(filename, 80, "screenshot_%y%m%d-%H%M%S.bmp", timeinfo);

	SDL_SaveBMP(screen, filename);
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
			// Toggle fullscreen X11
			if (!SDL_WM_ToggleFullScreen(screen)) {
				// More portable version of toggle
				screen = SDL_SetVideoMode(0, 0, 0, screen->flags ^ SDL_FULLSCREEN);
				// If toggle failed, switch back
				if (screen == NULL) 
					screen = SDL_SetVideoMode(0, 0, 0, flags);
			}
		}
		if(event->key.keysym.sym == SDLK_F11) {
			fps_counter_visible = !fps_counter_visible;
		}
		if(event->key.keysym.sym == SDLK_PRINT) {
			screenshot_next_flip = true;
		}
	}
	if(event->type == SDL_KEYDOWN || event->type == SDL_KEYUP || event->type == SDL_JOYAXISMOTION ||
		event->type == SDL_JOYBUTTONDOWN || event->type == SDL_JOYBUTTONUP || event->type == SDL_JOYHATMOTION) {
		last_activity = frame;
	}
}
#include "ClientNetworkMultiplayer.h"
#include "NetworkMultiplayer.h"
#include "CharacterSelect.h"
int Main::run(const Main::RunModes &runmode) {
	if(!init()) return 1;

	frame_delay = 1000 / FRAMES_PER_SECOND;
	frame = 0;

	load_options();

	fps_counter_timer->start();

	switch (runmode)
	{
		case Main::RunModes::ARCADE:
			{
				Menu menu;

				running = true;
				menu.run();
			}
			break;

		case Main::RunModes::SERVER:
			while (Server::active())
			{
				running = true;

				NetworkMultiplayer multiplayer;
				Level &level(Server::getInstance().getLevel());
				multiplayer.set_level(&level);

				// This is a little bit of a design flaw, need to refactor server a bit later.
				Server::getInstance().initializeGame(multiplayer);

				Server::getInstance().initializeLevel();

				Server::getInstance().listen();

				multiplayer.run();
			}
			break;
		case Main::RunModes::CLIENT:

			// While developing I found it easier to hardcode this all here, but this will need to be refactored now

			ClientNetworkMultiplayer clientgame;

			Level level;

			running = true;

			CharacterSelect cs(1, 1);
			cs.run();
			if (!cs.cancel) 
			{
				Player player(0, 0);				

				player.input = Main::instance->input[0];
				player.input->set_delay();
				player.set_character(cs.player_select[0]);

				clientgame.add_player(&player);

				screen = Main::instance->screen;

				SDL_Color foregroundColor = { 255, 255, 255 };
				SDL_Color backgroundColor = { 0, 0, 255 };
				SDL_Surface* textSurface = Main::text->render_text_medium("Hello World!");
				
				Uint32 begin = SDL_GetTicks();
				Uint32 calculatedLag = begin;
				int initialLagTests = INITIAL_LAG_TESTS;
				
				SDL_Event event;

				while (true) {
					while(SDL_PollEvent(&event))
						;
				
					if (!ServerClient::getInstance().isConnected()) {
						begin = SDL_GetTicks();
						static bool once = true;
						if (once) {
							once = false;

							ServerClient::getInstance().connect(clientgame, level, player);
						}

					}
					else {
						Uint32 current = SDL_GetTicks();

						if (initialLagTests > 0) {
							if (calculatedLag - current > 200) {
								calculatedLag += 200;
								ServerClient::getInstance().test();
								initialLagTests--;
							}
						}
						else {
							// Todo: refactor, merge with code in Poll()
							if (calculatedLag - current > 1000)
							{
								calculatedLag += 1000;
								ServerClient::getInstance().test();
							}
						}
					}

					// Output console
					size_t beginSize = Logger::console.size();
					if (beginSize > 20)
						beginSize = beginSize - 20;
					else
						beginSize = 0;

					SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 255));

					int textpos = 5;
					for_each(std::begin(Logger::console) + beginSize, std::end(Logger::console), [&](const string &str) {
						SDL_Rect textLocation = { 10, textpos, 0, 0 };
						textpos += 24;
						SDL_Surface* textSurface = Main::text->render_text_medium(str.c_str());
						SDL_BlitSurface(textSurface, NULL, screen, &textLocation);
						SDL_FreeSurface(textSurface);
					});

					ServerClient::getInstance().poll(1);

					Main::instance->flip(true);

					if (ServerClient::getInstance().getState() == ServerClient::State::INITIALIZED)
					{
						lag_ = &(ServerClient::getInstance().getLag());
						clientgame.run();
					}

				}
				SDL_FreeSurface(textSurface);

				clientgame.run();
			}
			break;
	}
	
	save_options();

	clean_up();

#ifdef _DEBUG
#ifdef WIN32
	_CrtDumpMemoryLeaks();
#endif
#endif
	return 0;
}


int main(int argc, char* args[]) 
{
	Main main;
	
	// In windows when clicking on a smashbattle:// link, the current working dir is not set.
	// Therefore extract it from args[0] and change work dir.
	string cwd(util::basedir(string(args[0])));
	log(format("changing current directory to: %s", cwd.c_str()), Logger::Priority::INFO);

#ifdef WIN32
	_chdir(cwd.c_str());
#else
	chdir(cwd.c_str());
#endif

	printf("%d\n");
	for (int i=0; i<argc; i++)
		printf("%d = %s\n",i, args[i]);

	if(argc > 1) {
		if(strcmp(args[1], "-f") == 0) {
			Main::flags |= SDL_FULLSCREEN;
		}
		
		// Usage smashbattle -s "TRAINING DOJO" 1100 --> start server on port 1100 with level "TRAINING DOJO"
		else if(strcmp(args[1], "-s") == 0 && argc > 3) {
			string level(string(args[2]).substr(0, 80));
			string strport(string(args[3]).substr(0, 5));
			log(format("program started with -s flag, parsed level %s and port %d", level.c_str(), stoi(strport)), Logger::Priority::INFO);
			
			Server::getInstance().setState(new ServerStateInitialize(level, stoi(strport)));

			return main.run(Main::RunModes::SERVER);
		}

		// Usage smashbattle smashbattle://localhost:1100/ --> connect to server at host localhost port 1100
		//  note that if you register the smashbattle:// protocol you can open the game by clicking/opening the link
		else {
			char host[80+1] = {0x00};
			char port[5+1] = {0x00};

			if (2 == sscanf(args[1], "smashbattle://%80[^:]:%5[0-9]/", host, port)) {
				log(format("initialized as client, connect to: %s && %s", host, port), Logger::Priority::INFO);

				ServerClient::getInstance().setHost(host);
				ServerClient::getInstance().setPort(atoi(port));

				return main.run(Main::RunModes::CLIENT);
			}
		}
	}
	return main.run(Main::RunModes::ARCADE);
}

void Main::load_options() {
	char filename[256];
	std::ifstream file;
	SaveHeader hdr;

#ifdef WIN32
	sprintf(filename, "%s\\%s", getenv("APPDATA"), "smashbattle.sav");
#else
	sprintf(filename, "%s/%s", getenv("HOME"), ".smashbattle");
#endif
	file.open(filename, std::ifstream::in | std::ifstream::binary);

	if(file.eof() || !file.is_open()) {
		audio->options.sound_volume = 100;
		audio->options.music_volume = 100;
		set_default_controlschemes();
		file.close();
		return;
	}

	file.read((char*)&hdr, sizeof(SaveHeader));
	
	if(hdr.signature != SAVE_SIGNATURE || hdr.version != SAVE_VERSION) {
		audio->options.sound_volume = 100;
		audio->options.music_volume = 100;
		set_default_controlschemes();
		file.close();
		return;
	}

	audio->load_options(&file);

	for(int i = 0; i < 4; i++) {
		input[i]->load_options(&file);
	}

	file.close();
}

void Main::save_options() {
	char filename[256];
	std::ofstream file;
	SaveHeader hdr;

#ifdef WIN32
	sprintf(filename, "%s\\%s", getenv("APPDATA"), "smashbattle.sav");
#else
	sprintf(filename, "%s/%s", getenv("HOME"), ".smashbattle");
#endif
	file.open(filename, std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);

	if(file.fail()) {
		file.close();
		printf("Error: Could not save settings\n");
		return;
	}

	hdr.signature = SAVE_SIGNATURE;
	hdr.version = SAVE_VERSION;

	file.write((char *)&hdr, sizeof(SaveHeader));

	audio->save_options(&file);

	for(int i = 0; i < 4; i++) {
		input[i]->save_options(&file);
	}

	file.close();
}

void Main::set_default_controlschemes() {
	// Player 1
	input[0]->enable_keyboard(true);

	input[0]->bind_key(SDLK_a, A_LEFT);
	input[0]->bind_key(SDLK_d, A_RIGHT);
	input[0]->bind_key(SDLK_w, A_UP);
	input[0]->bind_key(SDLK_s, A_DOWN);
	
	input[0]->bind_key(SDLK_w, A_JUMP);
	input[0]->bind_key(SDLK_LSHIFT, A_RUN);
	
	input[0]->bind_key(SDLK_LCTRL, A_SHOOT);
	input[0]->bind_key(SDLK_LALT, A_BOMB);

	input[0]->bind_key(SDLK_ESCAPE, A_START);

	input[0]->enable_joystick(true);
	input[0]->open_joystick(0);

	input[0]->bind_joyaxis(0, false, A_LEFT);
	input[0]->bind_joyaxis(0, true, A_RIGHT);
	input[0]->bind_joyaxis(1, false, A_UP);
	input[0]->bind_joyaxis(1, true, A_DOWN);

	input[0]->bind_joybutton(1, A_JUMP);
	input[0]->bind_joybutton(2, A_RUN);

	input[0]->bind_joybutton(5, A_SHOOT);
	input[0]->bind_joybutton(7, A_BOMB);

	input[0]->bind_joybutton(9, A_START);

	input[0]->bind_joyhat(0, SDL_HAT_LEFT, A_LEFT);
	input[0]->bind_joyhat(0, SDL_HAT_RIGHT, A_RIGHT);
	input[0]->bind_joyhat(0, SDL_HAT_UP, A_UP);
	input[0]->bind_joyhat(0, SDL_HAT_DOWN, A_DOWN);
	
	// Player 2
	input[1]->enable_keyboard(true);

	input[1]->bind_key(SDLK_LEFT, A_LEFT);
	input[1]->bind_key(SDLK_RIGHT, A_RIGHT);
	input[1]->bind_key(SDLK_UP, A_UP);
	input[1]->bind_key(SDLK_DOWN, A_DOWN);

	input[1]->bind_key(SDLK_UP, A_JUMP);
	input[1]->bind_key(SDLK_RSHIFT, A_RUN);
	
	input[1]->bind_key(SDLK_RCTRL, A_SHOOT);
	input[1]->bind_key(SDLK_RALT, A_BOMB);

	input[1]->bind_key(SDLK_RETURN, A_START);

	input[1]->enable_joystick(true);
	input[1]->open_joystick(1);

	input[1]->bind_joyaxis(0, false, A_LEFT);
	input[1]->bind_joyaxis(0, true, A_RIGHT);
	input[1]->bind_joyaxis(1, false, A_UP);
	input[1]->bind_joyaxis(1, true, A_DOWN);

	input[1]->bind_joybutton(1, A_JUMP);
	input[1]->bind_joybutton(2, A_RUN);

	input[1]->bind_joybutton(5, A_SHOOT);
	input[1]->bind_joybutton(7, A_BOMB);

	input[1]->bind_joybutton(9, A_START);

	input[1]->bind_joyhat(0, SDL_HAT_LEFT, A_LEFT);
	input[1]->bind_joyhat(0, SDL_HAT_RIGHT, A_RIGHT);
	input[1]->bind_joyhat(0, SDL_HAT_UP, A_UP);
	input[1]->bind_joyhat(0, SDL_HAT_DOWN, A_DOWN);

	// Players 3 and 4
	input[2]->enable_keyboard(false);
	input[2]->enable_joystick(false);
	input[3]->enable_keyboard(false);
	input[3]->enable_joystick(false);
}
