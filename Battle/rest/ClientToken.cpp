#include "ClientToken.h"

#include <iostream>
#include <cstring>
#include "util/json.h"

namespace rest {

ClientToken::ClientToken()
{
}

std::string ClientToken::get()
{
	json::Object obj = request("GET", "http://battle.cppse.nl/client/token");
	std::string tokenString(obj["random_string"]);

	return tokenString;
}

}