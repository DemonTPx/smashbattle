#pragma once

#include "OptionsScreen.h"
#include "util/json.h"

class ServerListing : public OptionsScreen
{
public:
	ServerListing(json::Array &array, Main &main);
	~ServerListing();

	void initialize();
	void run();
	void item_selected();

	json::Array servers_;

	Main &main_;
};
