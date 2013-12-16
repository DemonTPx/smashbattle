#pragma once

#include "OptionsScreen.h"

class AudioOptions : public OptionsScreen {
public:
	AudioOptions();

	void run();
	void item_selected();
	void selection_changed();
};
