#include "SDL/SDL.h"
#include <stdexcept>
#include "Player.h"

#include "network/Server.h"
#include "network/Commands.hpp"

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

	void update_tile(int tile_pos, int tile_hp)
	{

		if (Server::getInstance().active())
		{
			CommandUpdateTile command;
			command.data.time = Server::getInstance().getServerTime();
			command.data.tile = tile_pos;
			command.data.tile_hp = tile_hp;

			Server::getInstance().sendAll(command);
		}
	}
}
