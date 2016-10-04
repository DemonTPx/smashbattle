#include "SDL2/SDL.h"
#include "ServerListing.h"
#include "Options.h"
#include "util/stringutils.hpp"
#include "util/Log.h"
#include "network/ClientNetworkMultiplayer.h"
#include "network/ServerClient.h"

ServerListing::ServerListing(json::Array &servers, Main &main)
    : OptionsScreen(main), servers_(servers), main_(main)
{
	main_.runmode = MainRunModes::CLIENT;

	initialize();

	OptionsScreen::align = LEFT;
}

void ServerListing::initialize()
{
	// Make sure there isn't already a connection left over
	main_.getServerClient().disconnect();
	
	title = format("SERVERS FOUND: %-22d #PLAYERS/MAX", servers_.size());

	OptionItem * item;
	for (int i = 0, n = servers_.size(); i < n; i++) {
		json::Object obj = servers_[(size_t) i];
		std::string str = (std::string) obj["servername"];
		std::string activePlayers = (std::string) obj["activePlayers"];
		if (activePlayers.length() == 0 || activePlayers.length() > 1) {
			activePlayers.assign("0");
		}

		str.assign(format("%-45.45s #%s/4", str.c_str(), activePlayers.c_str()));
		item = new OptionItem();
		item->name = strdup(const_cast<char *> (str.c_str()));
		item->options = NULL;
		item->selected = 0;
		add_item(item);
	}
}

ServerListing::~ServerListing()
{
	main_.runmode = MainRunModes::ARCADE;
}

void ServerListing::run()
{
	OptionsScreen::run();
}

void ServerListing::item_selected()
{
	json::Object obj = servers_[(size_t)selected_item];
	std::cout << " Selected item is: " << (std::string)obj["host"] << ":" << (int)obj["port"] << std::endl;
	
	main_.getServerClient().setHost((std::string)obj["host"]);
	main_.getServerClient().setPort((int)obj["port"]);
	
	main_.reset_inputs();
	
	network::ClientNetworkMultiplayer clientgame(main_);
	clientgame.start();

	running = false;

	main_.reset_inputs();
}
