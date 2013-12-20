#include "Token.h"

#include <iostream>
#include <cstring>
#include "util/json.h"

namespace rest {

Token::Token()
{
}

std::string Token::get()
{
	json::Object obj = request("http://battle.cppse.nl/client/token");
	std::string tokenString(obj["random_string"]);

	return tokenString;
}

}