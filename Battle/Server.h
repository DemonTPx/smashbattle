#ifndef __SERVER_H__
#define __SERVER_H__

#include <map>
 
#include "SDL/SDL_net.h"

#include "Client.h"

class Server
{
public:

    static Server& getInstance()
    {
        static Server instance;
        return instance;
    }

	// Server listens
	void listen();

	// Server polls clients
	void poll();


private:
    Server();
	~Server();
	
	// Prevent copies of server
	Server(Server const&); // Don't implement
    void operator=(Server const&); // Don't implement

	SDLNet_SocketSet create_sockset();

	
	bool is_listening_;

	std::map<int, Client> clients_;
	
	TCPsocket server;

	IPaddress ip;
	TCPsocket sock;
	SDLNet_SocketSet set;
	Uint32 ipaddr;
	Uint16 port;

	// temp
	friend class Gameplay;
};
#endif // __SERVER_H__