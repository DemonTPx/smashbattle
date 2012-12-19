#include "SDL/SDL.h"

#include <stdexcept>
#include "Player.h"
#include "Level.h"


namespace level_util
{
	using std::string;
	string get_filename_by_name(string lvlname)
	{
		for (int i=0; i<Level::LEVEL_COUNT; i++)
			if (string(Level::LEVELS[i].name) == lvlname)
				return string(Level::LEVELS[i].filename);

		throw std::runtime_error("unknown level name");
	}

	void set_player_start(Player &player, const Level & level)
	{
		static int idx = 0;

		player.position->x = level.playerstart[idx].x * TILE_W + ((TILE_W - PLAYER_W) / 2);
		player.position->y = level.playerstart[idx].y * TILE_H - PLAYER_H;
		player.set_sprite((level.playerstart[idx].facing_right ? SPR_R : SPR_L));
		player.reset();

		if (++idx == 4)
			idx = 0;
	}
};

