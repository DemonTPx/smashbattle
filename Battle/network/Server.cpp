/**
 * The SDL_Net code could not have been so easily written without the examples provided by
 *  Jon C. Atkins, found here: http://jcatki.no-ip.org:8080/SDL_net/
 */
#include "network/Server.h"
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
	  ignoreClientInputUntil_(0),
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
#include "rest/ServerToken.h"
#include "rest/RegisterServer.h"
#include "util/stringutils.hpp"
#include "util/sha256.h"
void Server::registerServer()
{
	if (serverToken_.empty()) {
		rest::ServerToken token;
		try {
			serverToken_ = token.get();
		} catch (std::runtime_error &exception) {
			std::cout << " show_error(exception.what()); " << exception.what() << std::endl;
		}
	}

	// @Todo use another key
	char secretKey[] = {
		0x56, 0xda, 0xce, 0x87, 0x52, 0x85, 0x50, 0xf1, 0xdd, 0x0c, 0x86, 0x92, 0x33, 0x49,
		0x21, 0xf4, 0x92, 0x23, 0x2b, 0xf3, 0x0c, 0x31, 0x23, 0x0e, 0xae, 0x49, 0x83, 0x92,
		0x2a, 0xdf, 0x9c, 0x8f
	};

	std::string secretKeyString(secretKey, sizeof (secretKey));
	std::string secretKeyStringHex(string_to_hex(secretKeyString));

	unsigned char md[32];
	std::string inpstr = serverToken_, sha256randomhash;

	inpstr.append(secretKeyStringHex);
	sha256((void *) inpstr.c_str(), inpstr.length(), md);

	char temp[4 + 1] = {0x00};
	for (int i = 0; i < 32; i++) {
		sprintf(temp, "%02x", md[i]);
		sha256randomhash.append(temp);
	}

	std::cout << "our generated token is: " << sha256randomhash << std::endl;
	
	serverToken_ = sha256randomhash;
	
	rest::RegisterServer regsrv(serverToken_);
	regsrv.put();
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

	return Server::getInstance().currentState_->type().find("ServerStateInactive") == std::string::npos; 
}

bool Server::gameStarted()
{
	if (!Server::getInstance().currentState_)
		return false;

	return Server::getInstance().currentState_->type().find("ServerStateGameStarted") != std::string::npos;
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
