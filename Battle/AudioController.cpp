#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"

#include "Main.h"
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
	mus_title = Mix_LoadMUS("music/title.ogg");
	mus_battle = Mix_LoadMUS("music/battle.ogg");

	select = Mix_LoadWAV("sfx/select.wav");
	pause = Mix_LoadWAV("sfx/pause.wav");
	
	select_character = Mix_LoadWAV("sfx/select_character.wav");

	shoot = Mix_LoadWAV("sfx/shoot.wav");
	jump = Mix_LoadWAV("sfx/jump.wav");
	hit = Mix_LoadWAV("sfx/hit.wav");
	bounce = Mix_LoadWAV("sfx/bounce.wav");

	item = Mix_LoadWAV("sfx/item.wav");

	youlose = Mix_LoadWAV("sfx/youlose.wav");

	countdown = Mix_LoadWAV("sfx/countdown.wav");
	go = Mix_LoadWAV("sfx/go.wav");
}

void AudioController::close_files() {
	Mix_FreeMusic(mus_title);
	Mix_FreeMusic(mus_battle);

	Mix_FreeChunk(select);
	Mix_FreeChunk(pause);

	Mix_FreeChunk(select_character);

	Mix_FreeChunk(shoot);
	Mix_FreeChunk(jump);
	Mix_FreeChunk(hit);
	Mix_FreeChunk(bounce);

	Mix_FreeChunk(item);

	Mix_FreeChunk(youlose);

	Mix_FreeChunk(countdown);
	Mix_FreeChunk(go);
}

void AudioController::play_music(int music) {
	if(!Main::music_on)
		return;

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
	if(!Main::sound_on)
		return;

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

void AudioController::play(int sound) {
	switch(sound) {
		case SND_SELECT:
			play_sound(select, 50);
			break;
		case SND_PAUSE:
			play_sound(pause, 50);
			break;
		case SND_SELECT_CHARACTER:
			play_sound(select_character, 50);
			break;
		case SND_JUMP:
			play_sound(jump);
			break;
		case SND_SHOOT:
			play_sound(shoot);
			break;
		case SND_HIT:
			play_sound(hit);
			break;
		case SND_BOUNCE:
			play_sound(bounce);
			break;
		case SND_ITEM:
			play_sound(item, 75);
			break;
		case SND_YOULOSE:
			play_sound(youlose);
			break;
		case SND_COUNTDOWN:
			play_sound(countdown);
			break;
		case SND_GO:
			play_sound(go);
			break;
	}
}

