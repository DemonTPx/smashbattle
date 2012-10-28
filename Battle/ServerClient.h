#ifndef __SERVER_CLIENT_H__
#define __SERVER_CLIENT_H__

#include <string.h>
#include <sys/types.h>
#ifndef WIN32
#include <unistd.h>
#include <sys/time.h>
#else
#include <windows.h>
#include <time.h>
#endif
#include <stdlib.h>
#include "SDL/SDL.h"
#include "SDL/SDL_thread.h"
#include "SDL/SDL_net.h"

class ServerClient
{
public:
	
	static const short FLAG_LEFT = 0x02;
	static const short FLAG_RIGHT = 0x04;
	static const short FLAG_UP = 0x08;
	static const short FLAG_DOWN = 0x10;
	static const short FLAG_JUMP = 0x20;
	static const short FLAG_RUN = 0x40;
	static const short FLAG_SHOOT = 0x80;
	static const short FLAG_BOMB = 0x100;
	static const short FLAG_START = 0x200;
	

	static ServerClient& getInstance()
	{
		static ServerClient instance;
		return instance;
	}

	void connect();
	void poll(short test);

private:
	ServerClient();
	~ServerClient();
	
	ServerClient(ServerClient const&); // Don't implement
	void operator=(ServerClient const&); // Don't implement

	bool is_connected_;

	IPaddress ip;
	TCPsocket sock;
	Uint16 port;
	SDLNet_SocketSet set;
	fd_set fdset;
	int result;
	char *name,*str;
	struct timeval tv;
};


#endif // __SERVER_CLIENT_H__