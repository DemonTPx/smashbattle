#include "network/Client.h"
#include "network/Server.h"
#include "network/Commands.hpp"
#include "util/Log.h"
#include "util/random.h"
#include "NetworkMultiplayer.h"
#include "Projectile.h"
#include "Bomb.h"
#include "Main.h"

#include <sstream>

namespace network {

Client::Client(Client &&other) 
	: CommandProcessor(NULL),
	  test(0),

	  lag_(INITIAL_LAG_TESTS), 
	  lastLagTime_(0),
	  initialLagTests_(INITIAL_LAG_TESTS),
	  server_((Server *)0x4321),

	  currentState_(Client::State::CONNECTING),
	  commToken_(other.getCommToken()),
	  lastUdpSeq_(0),
	  main_(other.main_)
{
	// UDP initialize
	if (!(p = SDLNet_AllocPacket(4096))) {
		fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
	}
}

Client & Client::operator=(Client&& other)
{
	if (this != &other)
	{
		// The parameters from 'normal' constructor
		this->client_id_ = other.client_id_;
		this->set_socket(other.socket_);
		this->server_ = other.server_;
		this->lastUdpSeq_ = other.lastUdpSeq_;
	}
	
	return *this; 
}

/**
 * Our intended constructor :P
 */
Client::Client(int client_id, TCPsocket socket, Server * const server, Main &main)
	: CommandProcessor(socket),
	  
	  client_id_(client_id),
	  character_(0),
	  server_(server) ,
	  test(0),

	  lag_(INITIAL_LAG_TESTS),
	  lastLagTime_(0),
	  initialLagTests_(10),

	  currentState_(Client::State::CONNECTING),
	  commToken_(rand_get()),
	  lastUdpSeq_(0),
	  main_(main)
{
	if (!(p = SDLNet_AllocPacket(4096))) {
		fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
	}
}

Client::~Client()
{
	SDLNet_FreePacket(p);
}

bool Client::process(std::unique_ptr<Command> command)
{
	Command *cmd = command.get();

	switch (cmd->getType())
	{
		case Command::Types::Ping:
			return process(dynamic_cast<CommandPing *>(cmd));
		case Command::Types::Pong:
			return process(dynamic_cast<CommandPong *>(cmd));
		case Command::Types::SetCharacter:
			return process(dynamic_cast<CommandSetCharacter *>(cmd));
		case Command::Types::SetPlayerData:
			return process(dynamic_cast<CommandSetPlayerData *>(cmd));
		case Command::Types::ShotFired:
			return process(dynamic_cast<CommandShotFired *>(cmd));
		case Command::Types::BombDropped:
			return process(dynamic_cast<CommandBombDropped *>(cmd));
		case Command::Types::CommunicationTokenAck:
			return process(dynamic_cast<CommandCommunicationTokenAck *>(cmd));
		case Command::Types::SetClientReady:
			return process(dynamic_cast<CommandSetClientReady *>(cmd));
		case Command::Types::ApiPing:
			return process(dynamic_cast<CommandApiPing *>(cmd));
	}

	return true;
}

bool Client::process(CommandPing *command)
{
	CommandPong response;
	response.data.time = command->data.time;

	send(response);

	return true;
}

bool Client::process(CommandPong *command)
{
	float lag = static_cast<float>((server_->getServerTime() - command->data.time) / 2.);

	lag_.add(lag);

	return true;
}

bool Client::process(CommandSetCharacter *command)
{
	char character = command->data.character;
	character_ = character;

	auto &myplayer = player_util::get_player_by_id(main_, client_id_);

	myplayer.character = character;

	myplayer.set_sprites();

	setState(Client::State::CHARACTER_INITIALIZED);

	return true;
}

bool Client::process(CommandSetPlayerData *command)
{
	if (server_->getGame().is_ended() || server_->ignoreClientInput())
		return true;
	
	Player *updatedPlayer = NULL;
	auto playersvec = *(server_->getGame().players);
	for (auto i = playersvec.begin(); i != playersvec.end(); i++)
	{
		auto &player = **i;
		if (player.number == (int)command->data.client_id) {
			short lastSeq = server_->getClientById(player.number)->getLastUdpSeq();
			short currSeq = command->data.udp_sequence;

			// A following sequence is valid if it's bigger than the lastSeq.
			// But it's also valid if it's smaller, (i.e. wrapped, short is overflown). That's where 
			//  the check if the difference is in that case > sizeof(short) / 2)..
			if ( ! ((currSeq > lastSeq || (currSeq < lastSeq && (lastSeq-currSeq)) > (sizeof(short)/2)))) {
				log(format("sequence, discarding because of more recent package existant. %d < %d\n", currSeq, lastSeq), Logger::Priority::INFO);
				continue;
			}

			player_util::set_player_data(player, *command);

			// Account for lag
			int processFrames = static_cast<int>(lag().avg() / static_cast<float>(main_.MILLISECS_PER_FRAME));
			for (int i=0; i<processFrames; i++)
				server_->getGame().move_player(player);


			updatedPlayer = &player;
		}
	}

	// Notify other players of this new update
	if (!updatedPlayer)
		return true;

	for (auto i = playersvec.begin(); i != playersvec.end(); i++)
	{
		auto &player = **i;
		if (player.number != (int)command->data.client_id) {
			CommandSetPlayerData data;

			player_util::set_position_data(data, updatedPlayer->number, server_->getServerTime(), server_->getUdpSeq(), *updatedPlayer);

			try {
				auto client = server_->getClientById(player.number);
				if (client->getState() >= Client::State::ACTIVE) {
					client->send(data);
				}
			}
			catch (std::runtime_error &err) {
				// probably an udp packet received from a client that's already disconnected or something..
			}
		}
	}
	return true;
}

bool Client::process(CommandShotFired *command)
{
	// I'm currently too lazy to create function for it, I will refactor! (Todo!!)
	auto playersvec = *(server_->getGame().players);
	for (auto i = playersvec.begin(); i != playersvec.end(); i++)
	{
		auto &player = **i;
		if (player.number == client_id_)
		{
			// Make sure the correct sprite is set, so the bullet will go the correct direction
			player.current_sprite = command->data.current_sprite;

			Projectile *proj = player.create_projectile_for_player(command->data.x, command->data.y);

			proj->distance_traveled = command->data.distance_travelled;

			// Account for lag
			int processFrames = static_cast<int>(lag().avg() / static_cast<float>(main_.MILLISECS_PER_FRAME));
			for (int i=0; i<processFrames; i++)
			{
				if (!server_->getGame().process_gameplayobj(proj))
				{
					// Object is already gone, don't bother sending it to clients
					return true;
				}
			}

			// Send all others the update
			for (auto i = playersvec.begin(); i != playersvec.end(); i++)
			{
				auto &player = **i;
				if (player.number != client_id_)
				{
					command->data.time = server_->getServerTime();
					command->data.client_id = client_id_;
					command->data.x = proj->position->x;
					command->data.y = proj->position->y;
					server_->getClientById(player.number)->send(*command);
				}
			}

			return true;
		}
	}

	return true;
}

// TTL moet nog in bomb e.d.
bool Client::process(CommandBombDropped *command)
{
	// I'm currently too lazy to create function for it, I will refactor! (Todo!!)
	auto playersvec = *(server_->getGame().players);
	for (auto i = playersvec.begin(); i != playersvec.end(); i++)
	{
		auto &player = **i;
		if (player.number == client_id_)
		{
			// Make sure the correct sprite is set, so the bullet will go the correct direction
			player.current_sprite = command->data.current_sprite;

			GameplayObject *obj = player.create_bomb(command->data.x, command->data.y);

			// Account for lag
			int processFrames = static_cast<int>(lag().avg() / static_cast<float>(main_.MILLISECS_PER_FRAME));
			for (int i=0; i<processFrames; i++)
			{
				if (!server_->getGame().process_gameplayobj(obj))
				{
					// Object is already gone, don't bother sending it to clients
					return true;
				}
			}

			// Send all others the update
			for (auto i = playersvec.begin(); i != playersvec.end(); i++)
			{
				auto &player = **i;
				if (player.number != client_id_)
				{
					command->data.time = server_->getServerTime();
					command->data.client_id = client_id_;
					command->data.x = obj->position->x;
					command->data.y = obj->position->y;

					server_->getClientById(player.number)->send(*command);
				}
			}

			return true;
		}
	}

	return true;
}

bool Client::process(CommandCommunicationTokenAck *command)
{
	log(format("Client confirmation received for token"), Logger::Priority::CONSOLE);

	setState(Client::State::CALCULATING_LAG);

	return true;
}

bool Client::process(CommandSetClientReady *command)
{
	log(format("Client confirmed our server-ready packet"), Logger::Priority::CONSOLE);

	// i.e. It's now safe to assume we can send UDP packets for other player's data
	
	setState(Client::State::READY_FOR_POSITIONAL_DATA);

	return true;
}

bool Client::process(CommandApiPing *command)
{
	CommandApiPong pong;
	pong.data.time = command->data.time;
	pong.data.num_joined_players = '0' + server_->numJoinedClients();
	pong.data.num_active_players = '0' + server_->numActiveClients();

	send(pong);

	return true;
}

void Client::send(Command &command)
{
	char type = command.getType();
	
	if (type == Command::Types::SetPlayerData ||
		type == Command::Types::Ping ||
		type == Command::Types::Pong
	){
		log(format("Sending to client %d packet of type 0x%x over UDP with seq %d", client_id_, type, getUdpSeq()), Logger::Priority::DEBUG);
		size_t packetsize = command.getDataLen() + 1;
		log(format(".. with packet size = %d", packetsize), Logger::Priority::DEBUG);

		std::stringstream ss;
		ss << format("sending_udp[%d] = ", packetsize);

		
		char *packet = (char *)p->data;
		*packet = type;
		memcpy(packet + 1, command.getData(), command.getDataLen());

		
		for (int i=0; i<packetsize; i++) {
			char p[2] = {0x00};
			p[0] = packet[i];
			ss << format(" %x", *p);
		}
		ss << std::endl;
		log(ss.str(), Logger::Priority::DEBUG);
		
		p->data = (Uint8 *) packet;
		const IPaddress &origin = getUDPOrigin();
		p->address.host = origin.host;
		p->address.port = origin.port;

		p->len = packetsize;

		int numsent = SDLNet_UDP_Send(server_->getUdpSocket(), -1, p); // This sets the p->channel
		if(!numsent) {
			printf("SDLNet_UDP_Send^1: %s (delivery to: %d)\n", SDLNet_GetError(), -1);
			// do something because we failed to send
			// this may just be because no addresses are bound to the channel...
			//exit(1);
		}
		setNextUdpSeq();

	}
	else {
		log(format("Sending to client %d packet of type %d over TCP", client_id_, type), Logger::Priority::DEBUG);
		
		size_t result = SDLNet_TCP_Send(socket_, &type, sizeof(char));

		if(result < sizeof(char)) {
			if(SDLNet_GetError() && strlen(SDLNet_GetError())) /* sometimes blank! */
				log(format("SDLNet_TCP_Send: %s\n", SDLNet_GetError()), Logger::Priority::FATAL);
			return;
		}

		result = SDLNet_TCP_Send(socket_, command.getData(), command.getDataLen());

		if(result < sizeof(socket_)) {
			if(SDLNet_GetError() && strlen(SDLNet_GetError())) /* sometimes blank! */
				log(format("SDLNet_TCP_Send: %s\n", SDLNet_GetError()), Logger::Priority::FATAL);
			return;
		}
	}
}

void Client::cleanup()
{
	CommandDelPlayer cmd;
	cmd.data.time = server_->getServerTime();
	cmd.data.client_id = client_id_;
	server_->sendAll(cmd);

	server_->getGame().del_player_by_id(client_id_);
}

}