#pragma once

#include <map>
#include <memory>

#include "SDL/SDL_net.h"

#include "network/Client.h"
#include "Level.h"

class Gameplay;

#include "util/ServerUtil.h"

class NetworkMultiplayer;

namespace network{

class ServerState;

/**
 * \brief Server for smashbattle network games.
 * 
 * This is a singleton object that listens on a specific port on both TCP and UDP.
 * A server can have different states, depending on the state it will accept certain commands.
 */
class Server {
public:

	static Server& getInstance() {
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

	Level &getLevel() {
		return level_;
	}

	std::string getLevelName() {
		return levelName_;
	}
	void setLevel(std::string level);

	void setPort(int port) {
		port_ = (Uint16) port;
	};

	void setName(std::string name) {
		servername_ = name;
	};

	void initializeLevel();
	void registerServer();
	void initializeGame(NetworkMultiplayer &);

	Uint32 getServerTime() {
		return serverTime_;
	}

	Gameplay &getGame();
	std::shared_ptr<Client> getClientById(int client_id);
	size_t numActiveClients();

	void sendAll(Command &command);

	void ignoreClientInputFor(int ms) {
		ignoreClientInputUntil_ = serverTime_ + ms;
	}

	bool ignoreClientInput() {
		return ignoreClientInputUntil_ > serverTime_;
	}

	std::string getName() {
		return servername_;
	}

	int getPort() {
		return port_;
	}

	short getUdpSeq() {
		return udpsequence_;
	}

	void setNextUdpSeq() {
		udpsequence_++;
	}

	UDPsocket & getUdpSocket() {
		return sd;
	}
	
private:
	Server();
	~Server();

	// Prevent copies of server
	Server(Server const&); // Don't implement
	void operator=(Server const&); // Don't implement

	SDLNet_SocketSet create_sockset();


	bool is_listening_;

	std::map<int, std::shared_ptr<Client>> clients_;
	std::map<Uint32, int> communicationTokens_;

	TCPsocket server;

	IPaddress ip;
	TCPsocket sock;
	SDLNet_SocketSet set;
	Uint32 ipaddr;
	Uint16 port_;
	std::string servername_;

	::Level level_;
	std::string levelName_;

	NetworkMultiplayer *game_;

	const ServerState * currentState_;

	Uint32 serverTime_;
	Uint32 ignoreClientInputUntil_;

	std::string serverToken_;

	// UDP
	UDPsocket sd; /* Socket descriptor */
	UDPpacket *p; /* Pointer to packet memory */
	short udpsequence_;

	friend class ClientNetworkMultiplayer;
};

}