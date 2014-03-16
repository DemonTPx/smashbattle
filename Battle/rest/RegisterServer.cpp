#include "RegisterServer.h"

#include <iostream>
#include <cstring>
#include "util/json.h"
#include "network/Server.h"
#include "Main.h"

namespace rest {

RegisterServer::RegisterServer(const std::string &serverToken)
    : serverToken_(serverToken)
{
}

std::string RegisterServer::put(Main &main)
{
	json::Object postData;
	postData["servername"] = main.getServer().getName();
	// Host will be determined by the API server for now
	//  postData["host"] = Server::getInstance().getHost();
	postData["port"] = main.getServer().getPort();
	postData["level"] = main.getServer().getLevelName();
	postData["required_players"] = 2;
	
	std::cout << "post data generated is; " << json::Serialize(postData) << std::endl;
	json::Object obj = request("PUT", "http://battle.cppse.nl/server", serverToken_, json::Serialize(postData));
	std::string tokenString(obj["random_string"]);

	return tokenString;
}

}