#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"

#include "AudioOptions.h"

AudioOptions::AudioOptions(Main &main) : OptionsScreen("OPTIONS > SOUND AND MUSIC", main) {
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
	item->selected = main_.audio->options.sound_volume / 20;
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
	item->selected = main_.audio->options.music_volume / 20;
	add_item(item);

	OptionsScreen::align = LEFT;
}

void AudioOptions::run() {
	OptionsScreen::run();
}

void AudioOptions::selection_changed() {
	switch(selected_item) {
		case 0: // sound volume
			main_.audio->options.sound_volume = items->at(0)->selected * 20;
			main_.audio->play(SND_SELECT);
			break;
		case 1: // music volume
			main_.audio->options.music_volume = items->at(1)->selected * 20;
			if(main_.audio->options.music_volume == 0)
				main_.audio->stop_music();
			else
				main_.audio->play_music(MUSIC_TITLE);
			break;
	}
}
