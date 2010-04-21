#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"

#include <iostream>

#include "Main.h"
#include "AudioController.h"

#define AUDIO_CHANNELS 8

const char * AudioController::music_files[MUSICFILES] = {
	"music/hydro_dreams.ogg",
	"music/space_farer.ogg"
};

const char * AudioController::sound_files[SOUNDFILES] = {
	"sfx/select.wav",
	"sfx/pause.wav",
	"sfx/select_character.wav",
	"sfx/jump.wav",
	"sfx/shoot.wav",
	"sfx/hit.wav",
	"sfx/bounce.wav",
	"sfx/explode.wav",
	"sfx/item.wav",
	"sfx/youlose.wav",
	"sfx/countdown.wav",
	"sfx/go.wav",
	"sfx/airstrike.wav",
	"sfx/laser.wav",
	"sfx/shield.wav"
};

const int AudioController::soundvolume[SOUNDFILES] = {
	50, //select
	50, //pause
	50, //select_character
	100, //jump
	100, //shoot
	100, //hit
	100, //bounce
	100, //explode
	75, //item
	100, //youlose
	100, //countdown
	100, //go
	100, //airstrike
	100, //laser
	100 //shield
};

AudioController::AudioController() {
	options.sound_volume = 100;
	options.music_volume = 100;
}

AudioController::~AudioController() {
}

bool AudioController::open_audio() {
	int audio_rate = 22050;
	Uint16 audio_format = AUDIO_S16SYS;
	int audio_channels = 2;
	int audio_buffers = 512;

	if(!Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers))
		return false;


	Mix_AllocateChannels(AUDIO_CHANNELS);
	return true;
}

void AudioController::close_audio() {
	Mix_CloseAudio();
}

void AudioController::load_files() {
	for(int i = 0; i < MUSICFILES; i++) {
		music[i] = Mix_LoadMUS(music_files[i]);
	}

	for(int i = 0; i < SOUNDFILES; i++) {
		sound[i] = Mix_LoadWAV(sound_files[i]);
	}
}

void AudioController::close_files() {
	for(int i = 0; i < MUSICFILES; i++) {
		Mix_FreeMusic(music[i]);
	}

	for(int i = 0; i < SOUNDFILES; i++) {
		Mix_FreeChunk(sound[i]);
	}
}

void AudioController::play_music(int music) {
	if(options.music_volume == 0)
		return;

	if(music < 0 || music >= MUSICFILES)
		return;

	if(this->music[music] == NULL)
		return;

	Mix_HaltMusic();
	Mix_VolumeMusic(options.music_volume);
	Mix_PlayMusic(this->music[music], -1);
}

void AudioController::play_music(Mix_Music * m) {
	if(options.music_volume == 0)
		return;

	if(m == NULL)
		return;

	Mix_HaltMusic();
	Mix_VolumeMusic(options.music_volume);
	Mix_PlayMusic(m, -1);
}

void AudioController::stop_music() {
	Mix_HaltMusic();
}

void AudioController::pause_music() {
	Mix_PauseMusic();
}

void AudioController::unpause_music() {
	Mix_ResumeMusic();
}

// TODO: change these functions into something with constantes like:
// play_sound(SND_JUMP);

void AudioController::play(int sound, int x) {
	if(options.sound_volume == 0)
		return;

	if(sound < 0 || sound >= SOUNDFILES)
		return;

	if(this->sound[sound] == NULL)
		return;
	
	int chan;
	int volume;
	int window_middle;
	float x_step;
	int other_volume;

	chan = 0;
	while(Mix_Playing(chan))
		chan++;
	
	// Do not play the sound when we are out of channels
	if(chan >= AUDIO_CHANNELS)
		return;

	window_middle = WINDOW_WIDTH / 2;
	x_step = (float)0x7f / window_middle;

	if(x > WINDOW_WIDTH)
		x -= WINDOW_WIDTH;

	Mix_SetPanning(chan, 0x7f, 0x7f);
	if(x > -1 && x != window_middle) {
		if(x < window_middle) {
			other_volume = (int)(x_step * x);
			Mix_SetPanning(chan, 0xff - other_volume, other_volume);
		}
		if(x > window_middle){
			other_volume = (int)(x_step * (WINDOW_WIDTH - x));
			Mix_SetPanning(chan, other_volume, 0xff - other_volume);
		}
	}

	volume = (int)(((double)options.sound_volume / 100) * soundvolume[sound]);

	Mix_Volume(chan, volume);
	Mix_PlayChannel(chan, this->sound[sound], 0);
}

void AudioController::save_options(std::ostream * stream) {
	stream->write((char*)&options, sizeof(SoundOptions));
}

void AudioController::load_options(std::istream * stream) {
	stream->read((char*)&options, sizeof(SoundOptions));
}
