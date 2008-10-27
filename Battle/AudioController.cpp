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
	int audio_buffers = 512;

	if(!Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers))
		return false;


	Mix_AllocateChannels(16);
	return true;
}

void AudioController::close_audio() {
	Mix_CloseAudio();
}

void AudioController::load_files() {
	mus_title = Mix_LoadMUS("title.ogg");
	mus_battle = Mix_LoadMUS("battle.ogg");

	select = Mix_LoadWAV("select.wav");
	shoot = Mix_LoadWAV("shoot.wav");
	jump = Mix_LoadWAV("jump.wav");
	hit = Mix_LoadWAV("hit.wav");
	pause = Mix_LoadWAV("pause.wav");
	countdown = Mix_LoadWAV("countdown.wav");
	go = Mix_LoadWAV("go.wav");
}

void AudioController::close_files() {
	Mix_FreeMusic(mus_title);
	Mix_FreeMusic(mus_battle);

	Mix_FreeChunk(select);
	Mix_FreeChunk(shoot);
	Mix_FreeChunk(jump);
	Mix_FreeChunk(hit);
	Mix_FreeChunk(pause);
	Mix_FreeChunk(countdown);
	Mix_FreeChunk(go);
}

void AudioController::play_music(int music) {
	Mix_HaltMusic();
	if(music == MUSIC_TITLE) {
		Mix_PlayMusic(mus_title, -1);
	}
	if(music == MUSIC_BATTLE) {
		Mix_PlayMusic(mus_battle, -1);
	}
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

void AudioController::play_sound(Mix_Chunk * sound, int volume) {
	int chan;
	chan = 0;
	while(Mix_Playing(chan))
		chan++;
	Mix_PlayChannel(chan, sound, 0);
	Mix_Volume(chan, volume);
}

void AudioController::play_sound(Mix_Chunk * sound) {
	play_sound(sound, 100);
}

// TODO: change these functions into something with constantes like:
// play_sound(SND_JUMP);

void AudioController::play_select() {
	play_sound(select, 50);
}

void AudioController::play_jump() {
	play_sound(jump);
}

void AudioController::play_shoot() {
	play_sound(shoot);
}

void AudioController::play_hit() {
	play_sound(hit);
}

void AudioController::play_pause() {
	play_sound(pause, 50);
}

void AudioController::play_countdown() {
	play_sound(countdown);
}

void AudioController::play_go() {
	play_sound(go);
}

