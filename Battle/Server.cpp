/**
 * The SDL_Net code could not have been so easily written without the examples provided by
 *  Jon C. Atkins, found here: http://jcatki.no-ip.org:8080/SDL_net/
 */
#include "Server.h"

#include "SDL/SDL.h"
#include "SDL/SDL_net.h"

#include <vector>

#include <stdio.h>
#include <stdlib.h>

using std::map;
using std::vector;

Server::Server()
	: is_listening_(false)
{
}

Server::~Server()
{
	SDLNet_Quit();
}

void Server::listen()
{
	if (is_listening_)
		return;

	char *message=NULL;
	const char *host=NULL;
	
	/* initialize SDL_net */
	if(SDLNet_Init()==-1)
	{
		printf("SDLNet_Init: %s\n",SDLNet_GetError());
		return;
	}

	port=(Uint16)1099;

	/* Resolve the argument into an IPaddress type */
	if(SDLNet_ResolveHost(&ip,NULL,port)==-1)
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
	printf("Port       : %d\n",port);

	/* open the server socket */
	server=SDLNet_TCP_Open(&ip);
	if(!server)
	{
		printf("SDLNet_TCP_Open: %s\n",SDLNet_GetError());
		return;
	}

	is_listening_ = true;
}

void Server::poll() 
{
	if (!is_listening_)
		return;

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
			char *name=NULL;

			int nextId = 0;
			for (; clients_.find(nextId) != clients_.end(); nextId++)
				;
			clients_[nextId] = Client(nextId, sock);

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
			char buffer[512] = {0x00};
			int bytesReceived = SDLNet_TCP_Recv(client.socket(), buffer, sizeof(buffer));

			if (bytesReceived > 0)
			{
				client.receive(bytesReceived, buffer);
			}
			else
			{
				// Close the old socket, even if it's dead... 
				SDLNet_TCP_Close(client.socket());
				
				// Mark for deletion
				dead_clients.push_back(client.id());

				printf("Cleaned up client: %d\n", client.id());
			}
		}
	}

	// Remove deleted clients by key
	for (vector<int>::iterator i=dead_clients.begin(); i != dead_clients.end(); i++)
		clients_.erase(*i);
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