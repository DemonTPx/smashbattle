#include "SDL/SDL.h"
#include "Player.h"

#include "Server.h"

namespace server_util
{
	float get_lag_for(Player &player)
	{
		if (Server::getInstance().active())
			return Server::getInstance().getClientById(player.number).lag().avg();

		return 0.0;
	}
}