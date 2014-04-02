#pragma once

#include <vector>

#include "Level.h"
#include "Player.h"
#include "NPC.h"
#include "PauseMenu.h"
#include "GameplayObject.h"

class Gameplay {
public:
	Gameplay(Main &main);
	virtual ~Gameplay();

	int frame;

	Level * level;

	std::vector<Player*> * players;
	std::vector<NPC*> * npcs;
	
	std::vector<GameplayObject*> * objects;

	void run();
	void move_player(Player &player);
	bool process_gameplayobj(GameplayObject *projectile = NULL);

	void set_level(Level * level);
	void add_player(Player * player);
	void del_player_by_id(char number);
	void del_players();
	void del_other_players();
	void add_npc(NPC * npc);

	void add_object(GameplayObject * obj);
	void bounce_up_players_and_npcs(SDL_Rect * rect, SDL_Rect * source);

	static bool is_intersecting(SDL_Rect * one, SDL_Rect * two);

	bool is_ended() { return ended; }
	bool is_countdown() { return countdown; }
	void set_broadcast(std::string msg, int duration);
        void set_music_playing(bool set) { music_playing = set; }

	virtual void initialize();
	virtual void deinitialize();

	virtual void game_interrupted_reset() {}

protected:

	void reset_game();

	virtual void pause(Player * p);

	void process_countdown();
	
	virtual void draw_pause_screen();
	virtual void draw_score();
	virtual void draw_game_ended();
	virtual void draw_countdown();
	virtual void draw_disconnected();
	virtual void draw_console() {};
	virtual void draw_broadcast();

	virtual void on_game_reset() = 0;

	virtual void on_pre_processing() = 0;
	virtual void on_post_processing() = 0;
	virtual void on_input_handled() {};

	virtual void on_pre_delete_player(const Player &player) {}

	void process_player_collission();
	void process_npc_collission();
	void process_player_npc_collission();

	virtual void handle_pause_input(SDL_Event * event);
	
	Uint32 ticks_start;
	
	bool game_running;
	bool server_game_running;

	PauseMenu * pause_menu;

	bool countdown;
	int countdown_sec_left;
	int countdown_start;
	char countdown_pre_text[20];

	bool ended;
	int end_start;

	bool music_playing;

	SDL_Surface * screen;

	// Do players collide with each other?
	bool players_collide;
	// Do NPC's collide with each other?
	bool npcs_collide;
	// Do players collide with NPC's?
	bool players_npcs_collide;

	std::string broadcast_msg;
	int broadcast_duration;

	Uint32 disconnected_time_;

	Main &main_;
};
