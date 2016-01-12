#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"

#include <future>
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

#include "Gameplay.h"
#include "Airstrike.h"
#include "network/ServerClient.h"
#include "network/Server.h"

Main::Main()
:
	/**
	 * Used in 'main' gameloop where frame_delay is no longer used.
	 * A value of 17 would force the game to run in (1s = 1000ms / 17 =) ~58.82 fps
	 */
	MILLISECS_PER_FRAME(17),
	/**
	 * Used in all other places, like in menu, character set, these 'cap' to 60 fps.
	 */
	FRAMES_PER_SECOND(60),

	FRAMES_UNTIL_RESET(7200),

	CONTROLS_REPEAT_DELAY(30),
	CONTROLS_REPEAT_SPEED(10),

	JOYSTICK_AXIS_THRESHOLD(0x3fff),

	gameplay_(NULL),

	serverClient_(NULL),
	server_(NULL)

{
	serverClient_ = new network::ServerClient();
	server_ = new network::Server();

	screen = NULL;
	flags = SDL_SWSURFACE;

	running = false;
	frame_delay = 0;
	frame = 0;
	fps_cap = false;

	screenshot_next_flip = false;

	fps = NULL;

	fps_counter_last_frame = 0;
	fps_counter_this_frame = 0;
	fps_counter_timer = NULL;
	fps_counter_visible = false;
	ingame_debug_visible = false;

	audio = NULL;
	graphics = NULL;
	text = NULL;

	last_activity = 0;
	autoreset = true;
	is_reset = false;

	runmode = MainRunModes::ARCADE;
	no_sdl = false;
}

network::ServerClient & Main::getServerClient()
{
	return *serverClient_;
}

network::Server & Main::getServer()
{
	return *server_;
}

void Main::setGameplay(Gameplay *gameplay) {
	gameplay_ = gameplay;
}

Gameplay &Main::gameplay() {
	return *gameplay_;
}

Main::~Main() {
}

bool Main::init() {
	//Start SDL

	if (!no_sdl) {
		SDL_Init(SDL_INIT_EVERYTHING);

		SDL_Surface * icon;
		Uint8 * mask;

#ifdef TWEAKERS
		icon = Graphics::load_icon("gfx/sb-tweakers.bmp", &mask, 0x00ffff);
#else
		icon = Graphics::load_icon("gfx/sb-icon.bmp", &mask, 0x00ffff);
#endif
		SDL_WM_SetIcon(icon, mask);
		SDL_WM_SetCaption("Smash Battle", NULL);

		SDL_FreeSurface(icon);
		delete[] mask;
		
		screen = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32, flags);
		SDL_ShowCursor(0);
	}

	fps_cap = true;

	if (!no_sdl) {
		if(screen == NULL) return false;
	}
	
	fps = new Timer();

	fps_counter_timer = new Timer();

	audio = new AudioController(*this);
	audio->open_audio();
	audio->load_files();

	text = new Text();
	if (!no_sdl) {
		text->load_all();
	}

	graphics = new Graphics(*this);
	if (!no_sdl) {
		graphics->load_all();
	}

	if (!no_sdl) {
		SDL_JoystickEventState(SDL_ENABLE);
	}

	for(int i = 0; i < 4; i++) {
		input[i] = new GameInput(*this);
	}
	input_master = NULL;

	if (runmode == MainRunModes::SERVER) {
		// In case we need to do a level select we need to have a master input already
		input_master = input[0];
		input_master->set_delay(20);
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
	getServer().poll();

	getServerClient().poll();

	fps_count();

	if(screenshot_next_flip) {
		take_screenshot();
		screenshot_next_flip = false;
	}

	if (!no_sdl) {
		SDL_Flip(screen);
	}
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

		if (getServerClient().isConnected())
			sprintf(cap, "%d FPS %f LAG", fps_counter_this_frame, getServerClient().getLag().avg());
		else if (getServer().active())
			sprintf(cap, "%d FPS %s STATE", fps_counter_this_frame, getServer().getState()->type().c_str());
		else
			sprintf(cap, "%d FPS", fps_counter_this_frame);

		surf = text->render_text_small(cap);

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
		if (runmode == MainRunModes::SERVER)
			running = false;
	}

	/* Catch quit event and ALT-F4 */
	if(event->type == SDL_QUIT) {
		if (runmode != MainRunModes::SERVER)
			running = false;
	}
	if(event->type == SDL_KEYDOWN) {
		if(event->key.keysym.mod & KMOD_ALT) {
			if(event->key.keysym.sym == SDLK_F4 && runmode != MainRunModes::SERVER) {
				running = false;
			}
		}
		if(event->key.keysym.sym == SDLK_F1) {
			// Toggle console in case we're in client mode
			if (runmode == MainRunModes::CLIENT && getServerClient().isConnected())
				getServerClient().toggleConsole();
		}
		if(event->key.keysym.sym == SDLK_F5) {
			// Re-set server, accept client connects..
			if (runmode == MainRunModes::SERVER && getServer().active())
				getServer().setState(new network::ServerStateAcceptClients());
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
	this->runmode = runmode;

	if(!init()) return 1;


	frame_delay = 1000 / FRAMES_PER_SECOND;
	frame = 0;

	load_options();

	fps_counter_timer->start();

	switch (runmode)
	{
		case MainRunModes::ARCADE:
			{
				Menu menu(*this);

				running = true;
				menu.run();
			}
			break;

		case MainRunModes::SERVER:
			running = true;
			getServer().setMain(*this);
			
			LoggerLogToStdOut = true;

			while (getServer().active() && running)
			{

				NetworkMultiplayer multiplayer(*this);
				Level &level(getServer().getLevel());
				multiplayer.set_level(&level);

				// This is a little bit of a design flaw, need to refactor server a bit later.
				getServer().initializeGame(multiplayer);

				getServer().initializeLevel();
				
				getServer().listen();

				getServer().registerServer();

				multiplayer.run();
			}
			break;
		case MainRunModes::CLIENT:
			fps_counter_visible = true;

			getServer().setMain(*this);
			getServerClient().setCharacter((int)online_character);

			network::ClientNetworkMultiplayer clientgame(*this);

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
//	chdir("/usr/share/games/smashbattle/");
#endif

	if(argc > 1) {
		if(strcmp(args[1], "-f") == 0) {
			main.flags |= SDL_FULLSCREEN;
		}
		
		// Usage smashbattle -s "TRAINING DOJO" 1100 --> start server on port 1100 with level "TRAINING DOJO"
		else if(strcmp(args[1], "-s") == 0 && argc >= 2) {
			string level, strport, servername;
			bool hideSdlWindow = true;
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
			else if (argc >= 5) {
				// Get level from param
				level = string(args[2]).substr(0, 80);
				strport = string(args[3]).substr(0, 5);
				servername = string(args[4]).substr(0, 80);
				if (argc == 6) {
					hideSdlWindow = string(args[5]) == "true";
				}
			}

			log(format("program started with -s flag, parsed level %s and port %d", level.c_str(), stoi(strport)), Logger::Priority::INFO);
			
			main.getServer().setState(new network::ServerStateInitialize(level, stoi(strport), servername));

			main.no_sdl = hideSdlWindow;

			return main.run(MainRunModes::SERVER);
		}

		else if(strcmp(args[1], "-c") == 0 && argc >= 3) {
			char host[80+1] = {0x00};
			char port[5+1] = {0x00};
			std::string character = "INSPECTOR";
			if (argc >= 4) {
				character = args[3];
			}

			if (2 == sscanf(args[2], "%80[^:]:%5[0-9]/", host, port)) {
				log(format("initialized as client, connect to: %s && %s", host, port), Logger::Priority::INFO);

				main.getServerClient().setHost(host);
				main.getServerClient().setPort(atoi(port));
				main.getServerClient().setCharacter(main.getServerClient().characterByName(character));

				return main.run(MainRunModes::CLIENT);
			}
		}
	}

	// Launch server in separate thread
#ifdef ENABLE_EMBEDDED_SERVER
	auto f1 = std::async( std::launch::async, []{
		Main main;
		main.no_sdl = true;
		main_.getServer().setState(new network::ServerStateInitialize("TRAINING DOJO", 1111, "RAY'S ASYNC SERV"));
		return main.run(MainRunModes::SERVER);
	});

	// Launch client
	main.no_sdl = false;
#endif

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
		load_default_options();
		file.close();
		return;
	}

	file.read((char*)&hdr, sizeof(SaveHeader));
	
	if(hdr.signature != SAVE_SIGNATURE || hdr.version > SAVE_VERSION) {
		load_default_options();
		file.close();
		return;
	}

	audio->load_options(&file);

	for(int i = 0; i < 4; i++) {
		input[i]->load_options(&file, hdr.version);
	}

	if (!file.eof()) {
		file.read((char*)&online_character, sizeof(online_character));
	}
	else {
		srand(SDL_GetTicks());
		online_character = (Uint8)(rand() % Player::CHARACTER_COUNT);
	}

	file.close();
}

void Main::load_default_options()
{
	audio->options.sound_volume = 100;
	audio->options.music_volume = 100;
	set_default_controlschemes();
	srand(SDL_GetTicks());
	online_character = (Uint8)(rand() % Player::CHARACTER_COUNT);
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

	file.write((char *)&online_character, sizeof(online_character));

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
