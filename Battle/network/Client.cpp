#include "network/Client.h"
#include "network/Server.h"
#include "network/Commands.hpp"
#include "util/Log.h"
#include "NetworkMultiplayer.h"

/**
 * This constructor is required for usage in a std::map by the server
 * Only accessable by it through a friend relation
 */
Client::Client() 
	: CommandProcessor(NULL),
	  test(0),

	  lag_(INITIAL_LAG_TESTS), 
	  lastLagTime_(0),
	  initialLagTests_(INITIAL_LAG_TESTS),
	  server_(NULL),

	  currentState_(Client::State::CONNECTING)
	  
{}

Client::Client(Client &&other) 
	: CommandProcessor(NULL),
	  test(0),

	  lag_(INITIAL_LAG_TESTS), 
	  lastLagTime_(0),
	  initialLagTests_(INITIAL_LAG_TESTS),
	  server_(NULL),

	  currentState_(Client::State::CONNECTING)
	  
{}	

Client & Client::operator=(Client&& other)
{
	if (this != &other)
	{
		// The parameters from 'normal' constructor
		this->client_id_ = other.client_id_;
		this->set_socket(other.socket_);
		this->server_ = other.server_;
	}
	
	return *this; 
}

/**
 * Our intended constructor :P
 */
Client::Client(int client_id, TCPsocket socket, Server * const server)
	: CommandProcessor(socket),
	  
	  client_id_(client_id),
	  character_(0),
	  server_(server) ,
	  test(0),

	  lag_(INITIAL_LAG_TESTS),
	  lastLagTime_(0),
	  initialLagTests_(10),

	  currentState_(Client::State::CONNECTING)
	  
{
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
		if (player.number == (int)command->data.client_id)
		{
			player_util::set_player_data(player, *command);


			// Account for lag
			int processFrames = static_cast<int>(lag().avg() / static_cast<float>(Main::MILLISECS_PER_FRAME));
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
		if (player.number != (int)command->data.client_id)
		{
			CommandSetPlayerData data;

			player_util::set_position_data(data, updatedPlayer->number, server_->getServerTime(), *updatedPlayer);

			server_->getClientById(player.number).send(data);
		}
	}
	return true;
}

#include "Projectile.h"
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
			int processFrames = static_cast<int>(lag().avg() / static_cast<float>(Main::MILLISECS_PER_FRAME));
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
					server_->getClientById(player.number).send(*command);
				}
			}

			return true;
		}
	}

	return true;
}

#include "Bomb.h"
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
			int processFrames = static_cast<int>(lag().avg() / static_cast<float>(Main::MILLISECS_PER_FRAME));
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

					server_->getClientById(player.number).send(*command);
				}
			}

			return true;
		}
	}

	return true;
}


void Client::send(Command &command)
{
	//	if (!is_connected_)
	//    return;

	log(format("Sending to client %d packet of type %d", client_id_, expectRequestFor_), Logger::Priority::DEBUG);


	char type = command.getType();
	size_t result = SDLNet_TCP_Send(socket_, &type, sizeof(char));

	if(result < sizeof(char)) {
		if(SDLNet_GetError() && strlen(SDLNet_GetError())) /* sometimes blank! */
			printf("SDLNet_TCP_Send: %s\n", SDLNet_GetError());
		return;
	}

	result = SDLNet_TCP_Send(socket_, command.getData(), command.getDataLen());

	if(result < sizeof(socket_)) {
		if(SDLNet_GetError() && strlen(SDLNet_GetError())) /* sometimes blank! */
			printf("SDLNet_TCP_Send: %s\n", SDLNet_GetError());
		return;
	}
}

void Client::cleanup()
{
	server_->getGame().del_player_by_id(client_id_);

	auto playersvec = *(server_->getGame().players);
	for (auto i = playersvec.begin(); i != playersvec.end(); i++)
	{
		auto &player = **i;
		
		CommandDelPlayer cmd;
		cmd.data.time = server_->getServerTime();
		cmd.data.client_id = client_id_;

		server_->getClientById(player.number).send(cmd);
	}
}
