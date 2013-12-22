#pragma once

#include <map>
 
#include "SDL/SDL_net.h"

#include "network/Client.h"
#include "Level.h"

class ServerState;
class Gameplay;
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
	static bool gameStarted();

	void setState(const ServerState * const state);

	Level &getLevel() { return level_; }
	std::string getLevelName() { return levelName_; }
	void setLevel(std::string level);
	void setPort(int port) { port_ = (Uint16)port; };
	void setName(std::string name) { servername_ = name; };

	void initializeLevel();
	void registerServer();
	void initializeGame(NetworkMultiplayer &);

	Uint32 getServerTime() { return serverTime_; }

	Gameplay &getGame();
	Client& getClientById(int client_id);
	size_t numActiveClients();

	void sendAll(Command &command);

	void ignoreClientInputFor(int ms) { ignoreClientInputUntil_ = serverTime_ + ms; }
	bool ignoreClientInput() { return ignoreClientInputUntil_ > serverTime_; }
	
	std::string getName() { return servername_; }
	int getPort() { return port_; }

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
	std::string servername_;

	Level level_;
	std::string levelName_;

	NetworkMultiplayer *game_;

	const ServerState * currentState_;
	
	Uint32 serverTime_;
	Uint32 ignoreClientInputUntil_;

	std::string serverToken_;
	
	friend class ClientNetworkMultiplayer;
};