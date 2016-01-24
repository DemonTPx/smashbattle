#include "SDL/SDL.h"
#include <stdexcept>
#include "Player.h"

#include "network/Server.h"
#include "network/Commands.hpp"
#include "Main.h"

namespace server_util
{
	float get_lag_for(Main &main, Player &player)
	{
		try {
			if (main.getServer().active())
				return main.getServer().getClientById(player.number)->lag().avg();
		}
		catch (std::runtime_error &)
		{
			// Client probably just disconnected and the player object is not yet cleaned up.
			return -1.0;
		}

		return 0.0;
	}

	void update_tile(Main &main, int tile_pos, int tile_hp)
	{

		if (main.getServer().active())
		{
			network::CommandUpdateTile command;
			command.data.time = main.getServer().getServerTime();
			command.data.tile = tile_pos;
			command.data.tile_hp = tile_hp;

			main.getServer().sendAll(command);
		}
	}
}
