#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"

#include <cstdio>
#include <iostream>
#include <fstream>
#include <ctime>

#include "Timer.h"
#include "Menu.h"
#include "AudioController.h"
#include "Graphics.h"


#include "Main.h"
#include "network/ServerClient.h"
#include "network/Server.h"
#include "NetworkMultiplayer.h"
#include "network/ClientNetworkMultiplayer.h"

#include "states/ServerStates.h"

#include "util/StringUtils.h"
#include "util/Log.h"
#include "util/random.h"

using std::string;

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
bool Main::ingame_debug_visible = false;

AudioController * Main::audio = NULL;
Graphics * Main::graphics = NULL;
Text * Main::text = NULL;

unsigned int Main::last_activity = 0;
bool Main::autoreset = true;
bool Main::is_reset = false;

MainRunModes Main::runmode = MainRunModes::ARCADE;

Main::Main()
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

	if (Main::runmode == MainRunModes::SERVER) {
		// In case we need to do a level select we need to have a master input already
		Main::instance->input_master = input[0];
		Main::instance->input_master->set_delay(20);
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

	for(int i = 0; i < 4; i++) {
		delete input[i];
	}

	//Quit SDL
	SDL_Quit();
}

void Main::flip(bool no_cap) 
{
	network::Server::getInstance().poll();

	network::ServerClient::getInstance().poll();

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

		if (network::ServerClient::getInstance().isConnected())
			sprintf(cap, "%d FPS %f LAG", fps_counter_this_frame, network::ServerClient::getInstance().getLag().avg());
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

	/* A server can only be closed with ESCAPE */
	if(event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_ESCAPE) {
		if (Main::runmode == MainRunModes::SERVER)
			running = false;
	}

	/* Catch quit event and ALT-F4 */
	if(event->type == SDL_QUIT) {
		if (Main::runmode != MainRunModes::SERVER)
			running = false;
	}
	if(event->type == SDL_KEYDOWN) {
		if(event->key.keysym.mod & KMOD_ALT) {
			if(event->key.keysym.sym == SDLK_F4 && Main::runmode != MainRunModes::SERVER) {
				running = false;
			}
		}
		if(event->key.keysym.sym == SDLK_F1) {
			// Toggle console in case we're in client mode
			if (Main::runmode == MainRunModes::CLIENT && network::ServerClient::getInstance().isConnected())
				network::ServerClient::getInstance().toggleConsole();
		}
		if(event->key.keysym.sym == SDLK_F5) {
			// Re-set server, accept client connects..
			if (Main::runmode == MainRunModes::SERVER && network::Server::getInstance().active())
				network::Server::getInstance().setState(new network::ServerStateAcceptClients());
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
		if(event->key.keysym.sym == SDLK_F2) {
			ingame_debug_visible = !ingame_debug_visible;
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

void Main::reset_inputs()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {}
	for (int i = 0; i < 4; i++) {
		input[i]->reset();
	}
	input_master->reset();
}

int Main::run(const MainRunModes &runmode) 
{

	Main::runmode = runmode;

	if(!init()) return 1;


	frame_delay = 1000 / FRAMES_PER_SECOND;
	frame = 0;

	load_options();

	fps_counter_timer->start();

	switch (runmode)
	{
		case MainRunModes::ARCADE:
			{
				Menu menu;

				running = true;
				menu.run();
			}
			break;

		case MainRunModes::SERVER:
			running = true;
			while (network::Server::active() && running)
			{

				NetworkMultiplayer multiplayer;
				Level &level(network::Server::getInstance().getLevel());
				multiplayer.set_level(&level);

				// This is a little bit of a design flaw, need to refactor server a bit later.
				network::Server::getInstance().initializeGame(multiplayer);

				network::Server::getInstance().initializeLevel();
				
				network::Server::getInstance().listen();

				network::Server::getInstance().registerServer();

				multiplayer.run();
			}
			break;
		case MainRunModes::CLIENT:
			fps_counter_visible = true;

			network::ClientNetworkMultiplayer clientgame;

			clientgame.start();

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
	/*
	TCPsocket socket;
	Client test(1, socket, 	&network::Server::getInstance());
	
	char peer0_a[] = { 0x07 };
	char peer0_a1[] = {
	    0x00, 0x00, 0x00, 0x00, 0x55, 0x41, 0x00, 0x00,  
	    0x00, 0x00, 0xc1, 0x01, 0x14, 0x01, 0x00, 0x00,  
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
 	    0x00, 0xff, 0xff};
	
	char peer0_a2[] = {
		                  0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00 
	};
	char peer0_a3[] = {
                                0x0a, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00
	};
	char peer0_a4[] = {
                          0x00
	};
	
	
	char peer0_b[] = { 0x07 };
	char peer0_b1[] = {
	0x00, 0x00, 0x00, 0x00, 0x1f, 0x42, 0x00, 0x00, 
	0x00, 0x00, 0xc1, 0x01, 0x14, 0x01, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00 };
	
	test.receive(sizeof(peer0_a), peer0_a);
	test.receive(sizeof(peer0_a1), peer0_a1);
	test.receive(sizeof(peer0_a2), peer0_a2);
	while (test.parse());
	test.receive(sizeof(peer0_a3), peer0_a3);
	test.receive(sizeof(peer0_a4), peer0_a4);
	test.receive(sizeof(peer0_b), peer0_b);
	while (test.parse());
	test.receive(sizeof(peer0_b1), peer0_b1);
	while (test.parse());
	*/
	/*

	char peerall[] = { 0x07, 
	    0x00, 0x00, 0x00, 0x00, 0x55, 0x41, 0x00, 0x00,  
	    0x00, 0x00, 0xc1, 0x01, 0x14, 0x01, 0x00, 0x00,  
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
 	    0x00, 0xff, 0xff,
		                  0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00 ,
                                0x0a, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00,
                          0x00,
		0x07,
	0x00, 0x00, 0x00, 0x00, 0x1f, 0x42, 0x00, 0x00, 
	0x00, 0x00, 0xc1, 0x01, 0x14, 0x01, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00 };
	
	
	rand_init();
	
	size_t length = sizeof(peerall);
	size_t i = 0;
	while (i < length) {
		std::cout << " i < length = " << i << " < " << length << std::endl;
		int rand_nmbr = rand_get();
		std::cout << " if (rand_nmbr("<< rand_nmbr<< ") > (length("<<length<<") - i("<<i<<"))("<<(length - i)<<"))" << std::endl;
		if (rand_nmbr > (length - i))
			rand_nmbr = (length - i);
		
		std::cout << " rand_nmbr = " << rand_nmbr << std::endl;
		
		test.receive(rand_nmbr, (peerall + i));
		while (test.parse());
		size_t i_old = i;
		i += rand_nmbr;
		std::cout << " i(" << i_old << ") += " << rand_nmbr << " = " << i << std::endl;
	}
	
	
	
	return 0;*/
	
	Main main;
	
	// In windows when clicking on a smashbattle:// link, the current working dir is not set.
	// Therefore extract it from args[0] and change work dir.
	string cwd(util::basedir(string(args[0])));

#ifdef WIN32
	_chdir(cwd.c_str());
#else
	chdir("/usr/share/games/smashbattle/");
#endif

	if(argc > 1) {
		if(strcmp(args[1], "-f") == 0) {
			Main::flags |= SDL_FULLSCREEN;
		}
		
		// Usage smashbattle -s "TRAINING DOJO" 1100 --> start server on port 1100 with level "TRAINING DOJO"
		else if(strcmp(args[1], "-s") == 0 && argc >= 2) {
			string level, strport, servername;
			if (argc == 2) {
				strport = "1100";
			}
			else if (argc == 3) {
				strport = string(args[2]).substr(0, 5);
			}
			else if (argc == 4) {
				// Get level from param
				level = string(args[2]).substr(0, 80);
				strport = string(args[3]).substr(0, 5);
			}
			else if (argc == 5) {
				// Get level from param
				level = string(args[2]).substr(0, 80);
				strport = string(args[3]).substr(0, 5);
				servername = string(args[4]).substr(0, 80);
			}

			log(format("program started with -s flag, parsed level %s and port %d", level.c_str(), stoi(strport)), Logger::Priority::INFO);
			
			network::Server::getInstance().setState(new network::ServerStateInitialize(level, stoi(strport), servername));

			return main.run(MainRunModes::SERVER);
		}

		// Usage smashbattle smashbattle://localhost:1100/ --> connect to server at host localhost port 1100
		//  note that if you register the smashbattle:// protocol you can open the game by clicking/opening the link
		else {
			char host[80+1] = {0x00};
			char port[5+1] = {0x00};

			if (2 == sscanf(args[1], "smashbattle://%80[^:]:%5[0-9]/", host, port)) {
				log(format("initialized as client, connect to: %s && %s", host, port), Logger::Priority::INFO);

				network::ServerClient::getInstance().setHost(host);
				network::ServerClient::getInstance().setPort(atoi(port));

				return main.run(MainRunModes::CLIENT);
			}
		}
	}
	return main.run(MainRunModes::ARCADE);
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

	input[0]->bind_key(SDLK_LEFT, A_LEFT);
	input[0]->bind_key(SDLK_RIGHT, A_RIGHT);
	input[0]->bind_key(SDLK_UP, A_UP);
	input[0]->bind_key(SDLK_DOWN, A_DOWN);

	input[0]->bind_key(SDLK_UP, A_JUMP);
	input[0]->bind_key(SDLK_RSHIFT, A_RUN);
	
	input[0]->bind_key(SDLK_RCTRL, A_SHOOT);
	input[0]->bind_key(SDLK_RALT, A_BOMB);

	input[0]->bind_key(SDLK_RETURN, A_START);
	input[0]->bind_key(SDLK_BACKSPACE, A_BACK);

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
	input[0]->bind_joybutton(8, A_BACK);

	input[0]->bind_joyhat(0, SDL_HAT_LEFT, A_LEFT);
	input[0]->bind_joyhat(0, SDL_HAT_RIGHT, A_RIGHT);
	input[0]->bind_joyhat(0, SDL_HAT_UP, A_UP);
	input[0]->bind_joyhat(0, SDL_HAT_DOWN, A_DOWN);

	// Player 2
	input[1]->enable_keyboard(true);

	input[1]->bind_key(SDLK_a, A_LEFT);
	input[1]->bind_key(SDLK_d, A_RIGHT);
	input[1]->bind_key(SDLK_w, A_UP);
	input[1]->bind_key(SDLK_s, A_DOWN);

	input[1]->bind_key(SDLK_w, A_JUMP);
	input[1]->bind_key(SDLK_LSHIFT, A_RUN);

	input[1]->bind_key(SDLK_LCTRL, A_SHOOT);
	input[1]->bind_key(SDLK_LALT, A_BOMB);

	input[1]->bind_key(SDLK_ESCAPE, A_START);
	input[1]->bind_key(SDLK_BACKSPACE, A_BACK);

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
	input[1]->bind_joybutton(8, A_BACK);

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
