#include "SDL/SDL.h"
#include "Player.h"

#include "Server.h"

namespace server_util
{
	float get_lag_for(Player &player)
	{
		try {
			if (Server::getInstance().active())
				return Server::getInstance().getClientById(player.number).lag().avg();
		}
		catch (std::runtime_error &)
		{
			// Client probably just disconnected and the player object is not yet cleaned up.
			return -1.0;
		}

		return 0.0;
	}
}