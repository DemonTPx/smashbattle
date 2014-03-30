#include "NetworkMultiplayerRoundEnd.h"
#include "Main.h"

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
}