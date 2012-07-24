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

	void listen();
	void poll();
	
	
	bool do_test;
	bool test() { bool val = do_test; do_test = false; return val; }


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
};
#endif // __SERVER_H__