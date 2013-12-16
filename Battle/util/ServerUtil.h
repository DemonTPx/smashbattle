#pragma once

class Player;

namespace server_util
{
	float get_lag_for(Player &player);
	void update_tile(int tile_pos, int tile_hp);
}
