#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"

#include "AudioController.h"

AudioController::AudioController() {
}

AudioController::~AudioController() {
}

bool AudioController::open_audio() {
	int audio_rate = 22050;
	Uint16 audio_format = AUDIO_S16SYS;
	int audio_channels = 2;
	int audio_buffers = 1024;

	if(!Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers))
		return false;


	Mix_AllocateChannels(16);
	return true;
}

void AudioController::close_audio() {
	Mix_CloseAudio();
}

void AudioController::load_files() {
	select = Mix_LoadWAV("select.wav");
	shoot = Mix_LoadWAV("shoot.wav");
	hit = Mix_LoadWAV("hit.wav");
}

void AudioController::close_files() {
	Mix_FreeChunk(shoot);
}

void AudioController::play_select() {
	int chan;
	chan = 0;
	while(Mix_Playing(chan))
		chan++;
	Mix_PlayChannel(chan, select, 0);
}

void AudioController::play_shoot() {
	int chan;
	chan = 0;
	while(Mix_Playing(chan))
		chan++;
	Mix_PlayChannel(chan, shoot, 0);
}

void AudioController::play_hit() {
	int chan;
	chan = 0;
	while(Mix_Playing(chan))
		chan++;
	Mix_PlayChannel(chan, hit, 0);
}

