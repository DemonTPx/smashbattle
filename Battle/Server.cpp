/**
 * The SDL_Net code could not have been so easily written without the examples provided by
 *  Jon C. Atkins, found here: http://jcatki.no-ip.org:8080/SDL_net/
 */
#include "Server.h"
#include "states/ServerState.h"

#include "SDL/SDL.h"
#include "SDL/SDL_net.h"

#include <stdexcept>
#include <vector>
#include <iostream>
#include <string>

#include <stdio.h>
#include <stdlib.h>

using std::cout;
using std::endl;
using std::map;
using std::vector;
using std::string;

#include "NetworkMultiplayer.h"
#include "states/ServerStateAcceptClients.h"
#include "commands/CommandSetBroadcastText.hpp"

Server::Server()
	: is_listening_(false),
	  currentState_(NULL),
	  port_((Uint16)1099),
	  serverTime_(SDL_GetTicks()),
	  ignoreClientInputUntil_(SDL_GetTicks()),
	  game_(NULL)
{
}

Server::~Server()
{
	SDLNet_Quit();
	
	if (currentState_ != NULL)
		delete(currentState_);
}

void Server::setLevel(std::string level)
{
	// Cannot load level here yet, as it requires SDL to be initialized, etc.
	levelName_ = level;
	
}

#include "LevelSelect.h"
void Server::initializeLevel()
{
	if (levelName_ == "")
	{
		// A bit dirty to do this here, but it works :)
		LevelSelect ls;
		ls.run();
		levelName_.assign(Level::LEVELS[ls.level].name);

	}
	level_.load(level_util::get_filename_by_name(levelName_).c_str());
}

void Server::initializeGame(NetworkMultiplayer &game)
{
	game_ = &game;
}


Gameplay & Server::getGame()
{
	return *game_;
}

void Server::listen()
{
	if (is_listening_)
		return;

	const char *host=NULL;
	
	/* initialize SDL_net */
	if(SDLNet_Init()==-1)
	{
		printf("SDLNet_Init: %s\n",SDLNet_GetError());
		return;
	}

		/* Resolve the argument into an IPaddress type */
	if(SDLNet_ResolveHost(&ip,NULL,port_)==-1)
	{
		printf("SDLNet_ResolveHost: %s\n",SDLNet_GetError());
		return;
	}

	/* perform a byte endianess correction for the next printf */
	ipaddr=SDL_SwapBE32(ip.host);

	/* output the IP address nicely */
	printf("IP Address : %d.%d.%d.%d\n",
			ipaddr>>24,
			(ipaddr>>16)&0xff,
			(ipaddr>>8)&0xff,
			ipaddr&0xff);

	/* resolve the hostname for the IPaddress */
	host=SDLNet_ResolveIP(&ip);

	/* print out the hostname we got */
	if(host)
		printf("Hostname   : %s\n",host);
	else
		printf("Hostname   : N/A\n");

	/* output the port number */
	printf("Port       : %d\n",port_);

	/* open the server socket */
	server=SDLNet_TCP_Open(&ip);
	if(!server)
	{
		printf("SDLNet_TCP_Open: %s\n",SDLNet_GetError());
		return;
	}

	is_listening_ = true;

	setState(new ServerStateAcceptClients());
}

void Server::poll() 
{
	if (!is_listening_)
		return;


	// Update servertime
	serverTime_ = SDL_GetTicks();

	for (map<int, Client>::iterator i=clients_.begin(); i!=clients_.end(); i++)
	{
		Client &client(i->second);
		currentState_->execute(*this, client);
	}


	set=create_sockset();
	int numready=SDLNet_CheckSockets(set, 0);
	
	if(numready==-1)
		return (void)printf("SDLNet_CheckSockets: %s\n",SDLNet_GetError());

	if(!numready)
		return;

	// Accepting new clients
	if(SDLNet_SocketReady(server))
	{
		sock = SDLNet_TCP_Accept(server);
		if(sock)
		{
			int nextId = 0;
			for (; clients_.find(nextId) != clients_.end(); nextId++)
				;
			clients_[nextId] = Client(nextId, sock, this);

			printf("New client connected with id: %d\n", nextId);
		}
	}

	// Receive from clients
	vector<int> dead_clients;
	for (map<int, Client>::iterator i=clients_.begin(); i!=clients_.end(); i++)
	{
		Client &client(i->second);
		if(SDLNet_SocketReady(client.socket()))
		{
			char buffer[16384] = {0x00};
			int bytesReceived = SDLNet_TCP_Recv(client.socket(), buffer, sizeof(buffer));

			if (bytesReceived > 0)
			{
				client.receive(bytesReceived, buffer);
				while (client.parse())
					;
			}
			else
			{
				// Close the old socket, even if it's dead... 
				SDLNet_TCP_Close(client.socket());
				
				// Mark for deletion
				dead_clients.push_back(client.id());

				printf("Cleaned up client: %d\n", client.id());

				client.cleanup();
			}
		}
	}

	// Remove deleted clients by key
	for (vector<int>::iterator i=dead_clients.begin(); i != dead_clients.end(); i++)
		clients_.erase(*i);

	if (!dead_clients.empty())
	{
		CommandSetBroadcastText broadcast;
		broadcast.data.time = getServerTime();
		string text("ANOTHER PLAYER DISCONNECTED");
		strncpy(broadcast.data.text, text.c_str() , text.length());
		broadcast.data.duration = 2000;
		sendAll(broadcast);
	}
}

/* create a socket set that has the server socket and all the client sockets */
SDLNet_SocketSet Server::create_sockset()
{
	static SDLNet_SocketSet set=NULL;
	
	if (set)
		SDLNet_FreeSocketSet(set);

	set=SDLNet_AllocSocketSet(clients_.size()+1);
	if(!set) {
		printf("SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
		throw std::runtime_error("todo implement");
	}

	SDLNet_TCP_AddSocket(set, server);
	
	for (map<int, Client>::iterator i=clients_.begin(); i!=clients_.end(); i++)
		SDLNet_TCP_AddSocket(set, i->second.socket());
	
	return(set);
}

bool Server::active()
{
	if (!Server::getInstance().currentState_)
		return false;

	return Server::getInstance().currentState_->type() != "class ServerStateInactive const *"; 
}

bool Server::gameStarted()
{
	if (!Server::getInstance().currentState_)
		return false;

	return Server::getInstance().currentState_->type() == "class ServerStateGameStarted const *"; 
}

void Server::setState(const ServerState * const state)
{
	if (currentState_ != NULL)
		delete(currentState_);

	currentState_ = state;

	state->initialize(*this);

	active();
}

Client& Server::getClientById(int client_id)
{
	if (clients_.find(client_id) != clients_.end())
		return clients_[client_id];

	throw std::runtime_error("client not found by id");
}

size_t Server::numActiveClients()
{
	size_t num = 0;
	for (map<int, Client>::iterator i=clients_.begin(); i!=clients_.end(); i++)
		if (i->second.getState() == Client::State::ACTIVE)
			num++;

	return num;
}

void Server::sendAll(Command &command)
{
	for (map<int, Client>::iterator i=clients_.begin(); i!=clients_.end(); i++)
		i->second.send(command);
}
