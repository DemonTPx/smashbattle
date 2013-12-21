#pragma once

#include "OptionsScreen.h"
#include "util/json.h"

class ServerListing : public OptionsScreen
{
public:
	ServerListing(json::Array &array);
	~ServerListing();

	void run();
	void item_selected();

	json::Array servers_;
};
