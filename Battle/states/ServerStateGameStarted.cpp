#include "ServerStates.h"
#include "Server.h"
#include <vector>

#include "log.h"

#include "Commands.hpp"
#include "Player.h"
#include "NetworkMultiplayer.h"

using std::string;

ServerStateGameStarted::ServerStateGameStarted()
{
}

void ServerStateGameStarted::initialize(Server &server) const
{
}

// Todo : maybe more protocol related stuff should be in here,
//   and move the rest to pre and post processing in NetworkMultiplayer
void ServerStateGameStarted::execute(Server &server, Client &client) const
{
	Uint32 servertime = server.getServerTime();

	// Lag test every 1s
	Uint32 lagdiff = servertime - client.getLastLagTime();
	if (lagdiff >= 1000) {
		CommandPing ping;
		ping.data.time = servertime;
		client.send(ping);
		client.setLastLagTime(client.getLastLagTime() + 1000);
	}

	if (client.getState() != Client::State::ACTIVE)
	{
		return;// ignore for now
	}

	static bool once = true;
	if (once)
	{
		once = false;
		CommandSetBroadcastText broadcast;
		broadcast.data.time = server.getServerTime();
		string text("WELCOME YOU ARE NOW IN THE GAME");
		strncpy(broadcast.data.text, text.c_str() , text.length());
		broadcast.data.duration = 2000;
		server.sendAll(broadcast);
	}
}