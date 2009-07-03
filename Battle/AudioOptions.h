#ifndef _AUDIOOPTIONS_H
#define _AUDIOOPTIONS_H
#include "OptionsScreen.h"

class AudioOptions : public OptionsScreen {
public:
	AudioOptions();

	void run();
	void item_selected();
	void selection_changed();
};

#endif
