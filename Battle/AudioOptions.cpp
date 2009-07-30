#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"

#include "AudioOptions.h"

AudioOptions::AudioOptions() {
	OptionItem * item;

	item = new OptionItem();
	item->name = (char*)"AUDIO VOLUME";
	item->options = new std::vector<char *>(0);
	item->options->push_back((char*)"OFF");
	item->options->push_back((char*)"20");
	item->options->push_back((char*)"40");
	item->options->push_back((char*)"60");
	item->options->push_back((char*)"80");
	item->options->push_back((char*)"100");
	item->selected = Main::instance->audio->options.sound_volume / 20;
	add_item(item);

	item = new OptionItem();
	item->name = (char*)"MUSIC VOLUME";
	item->options = new std::vector<char *>(0);
	item->options->push_back((char*)"OFF");
	item->options->push_back((char*)"20");
	item->options->push_back((char*)"40");
	item->options->push_back((char*)"60");
	item->options->push_back((char*)"80");
	item->options->push_back((char*)"100");
	item->selected = Main::instance->audio->options.music_volume / 20;
	add_item(item);

	item = new OptionItem();
	item->name = (char*)"RETURN";
	item->options = NULL;
	item->selected = 0;
	add_item(item);

	OptionsScreen::align = LEFT;
}

void AudioOptions::run() {
	OptionsScreen::run();
}

void AudioOptions::item_selected() {
	switch(selected_item) {
		case 2:
			running = false;
			break;
	}
}

void AudioOptions::selection_changed() {
	switch(selected_item) {
		case 0: // sound volume
			Main::instance->audio->options.sound_volume = items->at(0)->selected * 20;
			Main::instance->audio->play(SND_SELECT);
			break;
		case 1: // music volume
			Main::instance->audio->options.music_volume = items->at(1)->selected * 20;
			if(Main::instance->audio->options.music_volume == 0)
				Main::instance->audio->stop_music();
			else
				Main::instance->audio->play_music(MUSIC_TITLE);
			break;
	}
}
