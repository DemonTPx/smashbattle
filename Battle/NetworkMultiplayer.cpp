#include "SDL/SDL.h"

#include "NetworkMultiplayer.h"
#include "Player.h"

void NetworkMultiplayer::on_game_reset()
{
	countdown = false;
}

void NetworkMultiplayer::on_post_processing()
{
	auto &vec = *players;
	for (auto i = vec.begin(); i != vec.end(); i++) {
		auto &player(**i);


		if (player.hitpoints <= 0)
			player.is_dead = true;
	}
}