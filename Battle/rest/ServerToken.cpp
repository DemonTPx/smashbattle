#include "ServerToken.h"

#include <iostream>
#include <cstring>
#include "util/json.h"

namespace rest {

ServerToken::ServerToken()
{
}

std::string ServerToken::get()
{
	json::Object obj = request("GET", "http://battle.cppse.nl/server/token");
	std::string tokenString(obj["random_string"]);

	return tokenString;
}

}