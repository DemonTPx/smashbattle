#pragma once

#include "Gameplay.h"

struct KillLog {
	Kill kill;
	int start_frame;
};

class LocalMultiplayer : public Gameplay {
public:

	LocalMultiplayer(Main &main);

	void set_ended(bool val) { this->ended = val; end_avatar_start_frame = frame; };
	void set_countdown(bool countdown, int seconds = -1);
	void set_draw(bool val) { this->draw = val; };
	void set_winner(Player &winner) { this->winner = &winner; }
	void unset_winner() { this->winner = NULL; }

	bool we_have_a_winner();

	int get_round() { return round; }

protected:
	virtual void initialize();

	virtual void on_game_reset();

	virtual void on_pre_processing();
	virtual void on_post_processing();
	virtual void on_pre_delete_player(const Player &player);

	virtual void draw_score();
	virtual void draw_game_ended();

	void draw_score_kills();

	void draw_score_duel();
	void draw_score_multi();

	virtual GameplayObject *generate_powerup(bool force = false);
	
	int end_avatar_start_frame;

	int round;

	Player * winner;
	bool draw;

	int powerup_rate;
	int powerup_max;

	int powerup_health_rate;
	int powerup_bullet_rate;
	int powerup_doubledamage_rate;
	int powerup_instantkill_rate;
	int powerup_bomb_rate;
	int powerup_mine_rate;
	int powerup_airstrike_rate;
	int powerup_laserbeam_rate;
	int powerup_shield_rate;
	int powerup_owlstrike_rate;
	int powerup_random_rate;

	std::vector<KillLog> kill_log_list;

	Main &main_;
};
