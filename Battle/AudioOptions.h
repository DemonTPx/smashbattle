#pragma once

#include "OptionsScreen.h"

class AudioOptions : public OptionsScreen {
public:
	AudioOptions();

	void run();
	void selection_changed();
};
