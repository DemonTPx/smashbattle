#ifndef _LOCALMULTIPLAYER_H
#define _LOCALMULTIPLAYER_H

class LocalMultiplayer : public Gameplay {
public:
protected:
	virtual void initialize();

	virtual void on_game_reset();

	virtual void on_pre_processing();
	virtual void on_post_processing();

	virtual void draw_score();
	virtual void draw_game_ended();

	void draw_score_duel();
	void draw_score_multi();

	void generate_powerup(bool force = false);
	
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
	int powerup_airstrike_rate;
	int powerup_laserbeam_rate;
	int powerup_shield_rate;
};

#endif
