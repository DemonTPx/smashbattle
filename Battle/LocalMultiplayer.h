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

	virtual void pause(Player * p);

	void draw_score_duel();
	void draw_score_multi();
	
	int end_avatar_start_frame;

	Player * winner;
	bool draw;
};

#endif
