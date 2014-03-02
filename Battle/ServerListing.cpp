#include "SDL/SDL.h"
#include "ServerListing.h"
#include "Options.h"
#include "util/stringutils.hpp"
#include "network/ClientNetworkMultiplayer.h"
#include "network/ServerClient.h"
#include <sstream>

ServerListing::ServerListing(json::Array &servers)
    : OptionsScreen(), servers_(servers)
{
	Main::runmode = MainRunModes::CLIENT;

	std::stringstream ss;
	ss << "SERVERS FOUND: " << servers.size();
	title = ss.str();

	OptionItem * item;
	for (int i = 0, n = servers.size(); i < n; i++) {
		json::Object obj = servers[(size_t) i];
		std::string str = (std::string) obj["servername"];
		item = new OptionItem();
		item->name = strdup(const_cast<char *> (str.c_str()));
		item->options = NULL;
		item->selected = 0;
		add_item(item);
	}

	OptionsScreen::align = LEFT;
}

ServerListing::~ServerListing()
{
	Main::runmode = MainRunModes::ARCADE;
}

void ServerListing::run()
{
	OptionsScreen::run();
}

void ServerListing::item_selected()
{
	json::Object obj = servers_[(size_t)selected_item];
	std::cout << " Selected item is: " << (std::string)obj["host"] << ":" << (int)obj["port"] << std::endl;
	
	network::ServerClient::getInstance().setHost((std::string)obj["host"]);
	network::ServerClient::getInstance().setPort((int)obj["port"]);
	
	Main::instance->reset_inputs();
	
	network::ClientNetworkMultiplayer clientgame;
	clientgame.start();

	Main::instance->reset_inputs();
}
