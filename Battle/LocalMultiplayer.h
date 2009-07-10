#ifndef _LOCALMULTIPLAYER_H
#define _LOCALMULTIPLAYER_H

class LocalMultiplayer : public Gameplay {
public:
protected:
	virtual void on_game_reset();

	virtual void on_pre_processing();
	virtual void on_post_processing();
	
	int end_avatar_start_frame;
};

#endif
