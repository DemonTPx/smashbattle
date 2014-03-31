#include "NetworkMultiplayerRoundEnd.h"
#include "Main.h"
#include "network/ServerClient.h"
#include "Gameplay.h"

NetworkMultiplayerRoundEnd::NetworkMultiplayerRoundEnd(Main &main, Uint32 displayMilliseconds)
  : LocalMultiplayerRoundEnd(main),
	displayMilliseconds_(displayMilliseconds),
	displayStartTime_(SDL_GetTicks())
{
}

NetworkMultiplayerRoundEnd::~NetworkMultiplayerRoundEnd()
{
}

void NetworkMultiplayerRoundEnd::do_run()
{
	ready = (SDL_GetTicks() - displayStartTime_  >= displayMilliseconds_);
	if (ready) {
		// Make sure that on the next countdown the level music will be started
		main_.getServerClient().getGame().set_music_playing(false);
	}
}

int NetworkMultiplayerRoundEnd::get_player_number(Player &player)
{
	// In Network multiplayer mode we start counting players from zero instead of one.
	return LocalMultiplayerRoundEnd::get_player_number(player) + 1; 
}