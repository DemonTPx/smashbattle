#ifndef _AUDIOCONTROLLER_H
#define _AUDIOCONTROLLER_H

class AudioController {
public:
	AudioController();
	~AudioController();
	bool open_audio();
	void close_audio();
	void load_files();
	void close_files();
	void play_select();
	void play_shoot();
	void play_hit();
private:
	Mix_Chunk * select;
	Mix_Chunk * shoot;
	Mix_Chunk * hit;
};

#endif
