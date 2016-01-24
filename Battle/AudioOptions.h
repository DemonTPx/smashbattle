#pragma once

#include "OptionsScreen.h"

class AudioOptions : public OptionsScreen {
public:
	AudioOptions(Main &main);

	void run();
	void selection_changed();
};
