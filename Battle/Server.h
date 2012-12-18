#ifndef __SERVER_H__
#define __SERVER_H__

#include <map>
 
#include "SDL/SDL_net.h"

#include "Client.h"
#include "Level.h"

class ServerState;
class NetworkMultiplayer;

#include "util/ServerUtil.h"

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

	static bool active();

	void setState(const ServerState * const state);

	Level &getLevel() { return level_; }
	std::string getLevelName() { return levelName_; }
	void setLevel(std::string level);
	void setPort(int port) { port_ = (Uint16)port; };

	void initializeLevel();
	void initializeGame(NetworkMultiplayer &);

	Uint32 getServerTime() { return serverTime_; }

	NetworkMultiplayer &getGame();
	Client& getClientById(int client_id);

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
	Uint16 port_;

	Level level_;
	std::string levelName_;

	NetworkMultiplayer *game_;

	const ServerState * currentState_;
	
	Uint32 serverTime_;

	// temp
	//friend class Gameplay;
	friend class ClientNetworkMultiplayer;
};
#endif // __SERVER_H__