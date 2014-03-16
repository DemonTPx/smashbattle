#pragma once

class Player;
class Main;

namespace server_util
{
	float get_lag_for(Main &main, Player &player);
	void update_tile(Main &main, int tile_pos, int tile_hp);
}
