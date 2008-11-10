#ifndef _OPTIONS_H
#define _OPTIONS_H
#include "OptionsScreen.h"

class Options : public OptionsScreen {
public:
	Options();

	void run();
	void exit_options();
	void item_selected();
};

#endif
