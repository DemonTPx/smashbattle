#include "SDL/SDL.h"
#include "ServerListing.h"
#include "Options.h"
#include "util/stringutils.hpp"

ServerListing::ServerListing(json::Array &servers)
    : servers_(servers)
{
	OptionItem * item;
	for (int i = 0, n = servers.size(); i < n; i++) {
		json::Object obj = servers[(size_t) i];
		std::string str = (std::string) obj["name"];
		item = new OptionItem();
		item->name = const_cast<char *> (str.c_str());
		item->options = NULL;
		item->selected = 0;
		add_item(item);
	}

	OptionsScreen::align = LEFT;
}

void ServerListing::run()
{
	OptionsScreen::run();
}

#include "ClientNetworkMultiplayer.h"
void ServerListing::item_selected()
{
	json::Object obj = servers_[(size_t)selected_item];
	std::cout << " Selected item is: " << (std::string)obj["host"] << ":" << (int)obj["port"] << std::endl;
	
	
	//fps_counter_visible = true;

	ClientNetworkMultiplayer clientgame;

	clientgame.start();
}
