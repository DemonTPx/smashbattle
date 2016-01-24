#pragma once

#include "OptionsScreen.h"

class Main;

class Options : public OptionsScreen {
public:
	Options(Main &main);

	void run();
	void item_selected();

	Main &main_;
};
