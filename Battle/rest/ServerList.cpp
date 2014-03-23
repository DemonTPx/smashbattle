#include "ServerList.h"

#include <iostream>
#include <curl/curl.h>
#include <cstring>
#include <stdexcept>
#include "util/json.h"


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
	json::Object obj = request("GET", "http://battle.cppse.nl/server/list", token_);

	if (!(bool)obj["return"]) {
		throw std::runtime_error(std::string(obj["error_message"]));
	}

	return obj["servers"];
}

}
