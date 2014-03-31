#include "ServerList.h"

#include <iostream>
#include <curl/curl.h>
#include <cstring>
#include <stdexcept>
#include <sstream>
#include "util/json.h"
#include "network/ServerClient.h"


namespace rest {

ServerList::ServerList(const std::string &token)
: token_(token)
{
}

ServerList::~ServerList()
{
}

json::Array ServerList::list()
{
	std::stringstream ss;
	ss << "http://battle.cppse.nl/server/list/" << ServerClientVersion;
	json::Object obj = request("GET", ss.str(), token_);

	if (!(bool)obj["return"]) {
		throw std::runtime_error(std::string(obj["error_message"]));
	}

	return obj["servers"];
}

}
