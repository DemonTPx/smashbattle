#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"

#include "Main.h"
#include "AudioController.h"

const char * AudioController::music_files[MUSICFILES] = {
	"music/title.ogg",
	"music/battle.ogg"
};

const char * AudioController::sound_files[SOUNDFILES] = {
	"sfx/select.wav",
	"sfx/pause.wav",
	"sfx/select_character.wav",
	"sfx/jump.wav",
	"sfx/shoot.wav",
	"sfx/hit.wav",
	"sfx/bounce.wav",
	"sfx/item.wav",
	"sfx/youlose.wav",
	"sfx/countdown.wav",
	"sfx/go.wav"
};

const int AudioController::soundvolume[SOUNDFILES] = {
	50, //select
	50, //pause
	50, //select_character
	100, //jump
	100, //shoot
	100, //hit
	100, //bounce
	75, //item
	100, //youlose
	100, //countdown
	100 //go
};

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
	if(!Main::music_on)
		return;

	if(music < 0 || music >= MUSICFILES)
		return;

	if(this->music[music] == NULL)
		return;

	Mix_HaltMusic();
	Mix_PlayMusic(this->music[music], -1);
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

void AudioController::play(int sound) {
	if(!Main::sound_on)
		return;

	if(sound < 0 || sound >= SOUNDFILES)
		return;

	if(this->sound[sound] == NULL)
		return;
	
	int chan;

	chan = 0;
	while(Mix_Playing(chan))
		chan++;
	Mix_PlayChannel(chan, this->sound[sound], 0);
	Mix_Volume(chan, soundvolume[sound]);
}
