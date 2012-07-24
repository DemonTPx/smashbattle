#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "SDL/SDL_net.h"
#include <string>
#include <map>

class Client
{
public:

	Client(int client_id, TCPsocket socket);
	
	void receive(int bytes_received, const char * const buffer);

	
	// accessors
	TCPsocket socket() { return socket_; }
	int id() { return client_id_; }
	

private:
	
	friend std::map<int, Client>;
	Client() {}	
	
	int client_id_;
	TCPsocket socket_;
	char buffer_[1024];
	int buffer_idx_;
};

#endif //__CLIENT_H__