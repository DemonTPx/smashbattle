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
#include "LevelSelect.h"
#include "rest/ServerToken.h"
#include "rest/RegisterServer.h"
#include "util/stringutils.hpp"
#include "util/sha256.h"
#include "util/Log.h"
#include "Client.h"

namespace network {

Server::Server()
	: is_listening_(false),
	  currentState_(NULL),
	  port_((Uint16) 1099),
	  serverTime_(SDL_GetTicks()),
	  ignoreClientInputUntil_(0),
	  game_(NULL),
	  udpsequence_(0),
	  set(NULL),
	  lastUpdateInApiTime_(serverTime_),
	  lastNumActivePlayers_(0)
{
}

Server::~Server() {
	// UDP cleanup
	SDLNet_FreePacket(p);

	// SQL Net cleanup
	SDLNet_Quit();

	if (currentState_ != NULL)
		delete(currentState_);
}

void Server::setLevel(std::string level) {
	// Cannot load level here yet, as it requires SDL to be initialized, etc.
	levelName_ = level;

}


void Server::initializeLevel() {
	if (levelName_ == "") {
		// A bit dirty to do this here, but it works :)
		LevelSelect ls(*main_);
		ls.run();
		levelName_.assign(Level::LEVELS[ls.level].name);

	}
	level_->load(level_util::get_filename_by_name(levelName_).c_str());
}

void Server::registerServer() {
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
	regsrv.put(*main_);
}

void Server::initializeGame(NetworkMultiplayer &game) {
	game_ = &game;
}

Gameplay & Server::getGame() {
	return *game_;
}

void Server::setMain(Main &main) 
{
	main_ = &main;
	level_ = std::shared_ptr<::Level>(new Level(main));
}

Main & Server::getMain() {
	return *main_;
}

void Server::listen() {

	if (is_listening_)
		return;

	/* initialize SDL_net */
	if (SDLNet_Init() == -1) {
		log(format("SDLNet_Init: %s\n", SDLNet_GetError()), Logger::Priority::FATAL);
		return;
	}

	/* Resolve the argument into an IPaddress type */
	if (SDLNet_ResolveHost(&ip, NULL, port_) == -1) {
		log(format("SDLNet_ResolveHost: %s\n", SDLNet_GetError()), Logger::Priority::FATAL);
		return;
	}

	/* perform a byte endianess correction for the next printf */
	ipaddr = SDL_SwapBE32(ip.host);

	/* output the IP address nicely */
	log(format("IP Address : %d.%d.%d.%d\n",
		ipaddr >> 24,
		(ipaddr >> 16)&0xff,
		(ipaddr >> 8)&0xff,
		ipaddr & 0xff), Logger::Priority::INFO);

	/* output the port number */
	log(format("Port       : %d\n", port_), Logger::Priority::DEBUG);

	/* open the server socket */
	server = SDLNet_TCP_Open(&ip);
	if (!server) {
		log(format("SDLNet_TCP_Open: %s\n", SDLNet_GetError()), Logger::Priority::DEBUG);
		return;
	}

	/* Open a socket */
	if (!(sd = SDLNet_UDP_Open(port_))) {
		fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

	/* Make space for the packet */
	if (!(p = SDLNet_AllocPacket(4096))) {
		fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

	is_listening_ = true;

	setState(new ServerStateAcceptClients());
}

void Server::poll() {

	// Update servertime
	serverTime_ = SDL_GetTicks();

	if (!is_listening_)
		return;

	// Every 10 seconds send update to API, or earlier if num active players changed
	if (!lastUpdateInApiTime_ || serverTime_ - lastUpdateInApiTime_ >= 10000 || lastNumActivePlayers_ != numActiveClients()) {
		rest::RegisterServer regsrv(serverToken_);
		regsrv.update(*main_);
		lastUpdateInApiTime_ = serverTime_;
		lastNumActivePlayers_ = numActiveClients();
	}
	
	set = create_sockset();

	while (SDLNet_UDP_Recv(sd, p)) {
		log(format("UDP Packet incoming\n"), Logger::Priority::DEBUG);
		log(format("\tChan:    %d\n", p->channel), Logger::Priority::DEBUG);
		log(format("\tData:    %s\n", (char *) p->data), Logger::Priority::DEBUG);
		log(format("\tFirst_char: %X\n", p->data[0]), Logger::Priority::DEBUG);

		Uint32 commToken = *(Uint32 *) (p->data + 1);
		if (communicationTokens_.find(commToken) != communicationTokens_.end()) {
			log(format("\tMatched client: %d\n", communicationTokens_[commToken]), Logger::Priority::DEBUG);
		} else {
			log(format("\tNo matching client..\n"), Logger::Priority::DEBUG);
			continue;
		}
		log(format("\tLen:     %d\n", p->len), Logger::Priority::DEBUG);
		log(format("\tMaxlen:  %d\n", p->maxlen), Logger::Priority::DEBUG);
		log(format("\tStatus:  %d\n", p->status), Logger::Priority::DEBUG);
		log(format("\tAddress: %x %x\n", p->address.host, p->address.port), Logger::Priority::DEBUG);

		auto &client = clients_[communicationTokens_[commToken]];

		client->setUDPOrigin(p->address);

		char *temp = (char *) p->data;
		temp += sizeof (Uint32); // skip one byte before the actual struct
		*temp = p->data[0];
		client->parse_udp(p->len - sizeof (Uint32), temp);
	}

	for (map<int, std::shared_ptr<Client>>::iterator i = clients_.begin(); i != clients_.end(); i++) {
		auto & client = i->second;
		currentState_->execute(*this, *client);
	}



	int numready = SDLNet_CheckSockets(set, 0);

	if (numready == -1)
		return (void) printf("SDLNet_CheckSockets: %s\n", SDLNet_GetError());

	if (!numready)
		return;

	// Accepting new clients
	if (SDLNet_SocketReady(server)) {
		sock = SDLNet_TCP_Accept(server);
		if (sock) {

			int nextId = 0;
			for (; clients_.find(nextId) != clients_.end(); nextId++)
				;

			if (nextId >= 4) {
				log(format("New client connected with id: %d [DENIED >= 4]\n", nextId), Logger::Priority::INFO);
				SDLNet_TCP_Close(sock);
				return;
			}

			// We now re-create the socket set every iteration
			//SDLNet_TCP_AddSocket(set, sock);

			clients_[nextId] = std::shared_ptr<Client>(new Client(nextId, sock, this, *main_));

			communicationTokens_[clients_[nextId]->getCommToken()] = nextId;

			log(format("New client connected with id: %d\n", nextId), Logger::Priority::INFO);

			IPaddress *remote_ip;
			remote_ip = SDLNet_TCP_GetPeerAddress(sock);
			if (!remote_ip) {
				printf("SDLNet_TCP_GetPeerAddress: %s\n", SDLNet_GetError());
				printf("This may be a server socket.\n");
			} else {
				// Bind address to the first free channel
				SDLNet_UDP_Unbind(sd, nextId);
				int channel = SDLNet_UDP_Bind(sd, nextId, remote_ip);
				if (channel == -1) {
					printf("SDLNet_UDP_Bind: %s\n", SDLNet_GetError());
					// do something because we failed to bind
				}
			}

		}
	}

	// Receive from clients
	vector<int> dead_clients;
	for (map<int, std::shared_ptr<Client>>::iterator i = clients_.begin(); i != clients_.end(); i++) {
		auto &client = i->second;
		if (SDLNet_SocketReady(client->socket())) {
			char buffer[16384] = {0x00};
			int bytesReceived = SDLNet_TCP_Recv(client->socket(), buffer, sizeof (buffer));

			if (bytesReceived > 0) {
				client->receive(bytesReceived, buffer);
				while (client->parse())
					;
			} else {

				// We now re-create the socket set every iteration
				// SDLNet_TCP_DelSocket(set, client->socket());

				// Close the old socket, even if it's dead... 
				SDLNet_TCP_Close(client->socket());

				// Mark for deletion
				dead_clients.push_back(client->id());

				log(format("Cleaning up client: %d\n", client->id()), Logger::Priority::INFO);
			}
		}
	}

	// Remove deleted clients by key
	bool anotherPlayerDisconnected = false;
	for (vector<int>::iterator i = dead_clients.begin(); i != dead_clients.end(); i++) {
		// Verify with a breakpoint if this is correct code...
		if (clients_[*i]->getState() == Client::State::ACTIVE || 
			clients_[*i]->getState() == Client::State::ACTIVE) 
		{
			anotherPlayerDisconnected = true;
		}
		clients_[*i]->cleanup();
		communicationTokens_.erase(clients_[*i]->getCommToken());
		clients_.erase(*i);
	}

	if (anotherPlayerDisconnected) {
		CommandSetBroadcastText broadcast;
		broadcast.data.time = getServerTime();
		string text("A PLAYER LEFT THE GAME");
		strncpy(broadcast.data.text, text.c_str(), text.length());
		broadcast.data.duration = 2000;
		sendAll(broadcast);
	}

	ServerState *newstate = getState()->check_self(*this);
	if (newstate) {
		setState(newstate);
	}

}

/* create a socket set that has the server socket and all the client sockets */
SDLNet_SocketSet Server::create_sockset() {


	if (set)
		SDLNet_FreeSocketSet(set);

	set = SDLNet_AllocSocketSet(clients_.size() + 1);

	SDLNet_TCP_AddSocket(set, server);

	std::map<int, std::shared_ptr<Client>>::iterator i;
	for (i=std::begin(clients_); i!=std::end(clients_); i++) {
		std::shared_ptr<Client> &client = (*i).second;

		SDLNet_TCP_AddSocket(set, client->socket());
	}

	if (!set) {
		printf("SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
		throw std::runtime_error("problem");
	}

	return set;
}

bool Server::active() {
	if (!currentState_)
		return false;

	return currentState_->type().find("ServerStateInactive") == std::string::npos;
}

bool Server::gameStarted() {
	if (!currentState_)
		return false;

	return currentState_->type().find("ServerStateGameStarted") != std::string::npos;
}

void Server::setState(const ServerState * const state) {
	if (currentState_ != NULL)
		delete(currentState_);

	currentState_ = state;

	state->initialize(*this);

	active();
}

const ServerState *Server::getState()
{
	return currentState_;
}

std::shared_ptr<Client> Server::getClientById(int client_id) {
	if (clients_.find(client_id) != clients_.end())
		return clients_[client_id];

	throw std::runtime_error("client not found by id");
}

size_t Server::numJoinedClients() {
	size_t num = 0;
	for (map<int, std::shared_ptr<Client>>::iterator i = clients_.begin(); i != clients_.end(); i++)
		if (i->second->getState() == Client::State::ACTIVE || i->second->getState() == Client::State::SPECTATING)
			num++;

	return num;
}

size_t Server::numActiveClients() {
	size_t num = 0;
	for (map<int, std::shared_ptr<Client>>::iterator i = clients_.begin(); i != clients_.end(); i++)
		if (i->second->getState() == Client::State::ACTIVE)
			num++;

	return num;
}

void Server::sendAll(Command &command) {
	for (map<int, std::shared_ptr<Client>>::iterator i = clients_.begin(); i != clients_.end(); i++)
		i->second->send(command);
}

}