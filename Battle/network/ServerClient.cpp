/**
 * The SDL_Net code could not have been so easily written without the examples provided by
 *  Jon C. Atkins, found here: http://jcatki.no-ip.org:8080/SDL_net/
 */

#include <algorithm>

#include "network/ServerClient.h"

#include "network/Commands.hpp"
#include "network/ClientNetworkMultiplayer.h"
#include "Level.h"

#include "util/Log.h"

#include "Projectile.h"
#include "Bomb.h"

namespace network {

using std::for_each;
using std::begin;
using std::end;

ServerClient::ServerClient()
	: is_connected_(false),
	  show_console_(false),
	  currentState_(ServerClient::State::INITIALIZING),
	  host_("localhost"),
	  port_((Uint16)1099),
	  CommandProcessor(0),
	  lag(INITIAL_LAG_TESTS),
	  game_(NULL),
	  level_(NULL),
	  my_id_(0x00),
	  lastResetTimer_(0),
	  resumeGameWithCountdown_(true),
	  resumeGameTime_(0),
	  character_(0)
{
}

ServerClient::~ServerClient()
{
	SDLNet_Quit();
}

void ServerClient::connect(ClientNetworkMultiplayer &game, Level &level, Player &player)
{
	if (is_connected_)
		return;

	game_ = &game;
	level_ = &level;
	player_ = &player;

	log(format("CONNECTING TO %s:%d...",host_.c_str(),port_), Logger::Priority::CONSOLE);

	/* initialize SDL */
	if(SDL_Init(0)==-1)
	{
		throw std::runtime_error(format("SDL_Init: %s\n",SDL_GetError()));
	}

	/* initialize SDL_net */
	if(SDLNet_Init()==-1)
	{
		throw std::runtime_error(format("SDLNet_Init: %s\n",SDLNet_GetError()));
	}

	set=SDLNet_AllocSocketSet(1);
	if(!set)
	{
		throw std::runtime_error(format("SDLNet_AllocSocketSet: %s\n", SDLNet_GetError()));
	}


	/* Resolve the argument into an IPaddress type */
	if(SDLNet_ResolveHost(&ip,host_.c_str(),port_)==-1)
	{
		throw std::runtime_error(format("SDLNet_ResolveHost: %s\n",SDLNet_GetError()));
	}
	
	/* open the server socket */
	sock=SDLNet_TCP_Open(&ip);
	if(!sock)
	{
		throw std::runtime_error(format("SDLNet_TCP_Open: %s\n",SDLNet_GetError()));
	}
	
	if(SDLNet_TCP_AddSocket(set,sock)==-1)
	{
		throw std::runtime_error(format("SDLNet_TCP_AddSocket: %s\n",SDLNet_GetError()));
	}
	


	if (!(sd = SDLNet_UDP_Open(0))) {
		fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}
	if(SDLNet_ResolveHost(&ip2, host_.c_str(),port_)==-1)
	{
		throw std::runtime_error(format("SDLNet_ResolveHost: %s\n",SDLNet_GetError()));
	}
	
	/* Make space for the packet */
	if (!(p = SDLNet_AllocPacket(4096)))
	{
		fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}
	
	log(format("CONNECTION SUCCESFUL",host_.c_str(),port_), Logger::Priority::CONSOLE);

	set_socket(sock);

	is_connected_ = true;
}

void ServerClient::disconnect()
{
	if (!is_connected_)
		return;
	
	SDLNet_TCP_Close(sock);
	is_connected_ = false;
}

void ServerClient::poll()
{
	while (SDLNet_UDP_Recv(sd, p)) {
		log(format("UDP Packet incoming\n"), Logger::Priority::DEBUG);
		log(format("\tChan:    %d\n", p->channel), Logger::Priority::DEBUG);
		log(format("\tData:    %s\n", (char *) p->data), Logger::Priority::DEBUG);
		log(format("\tFirst_char: %X\n", p->data[0]), Logger::Priority::DEBUG);
		log(format("\tLen:     %d\n", p->len), Logger::Priority::DEBUG);
		log(format("\tMaxlen:  %d\n", p->maxlen), Logger::Priority::DEBUG);
		log(format("\tStatus:  %d\n", p->status), Logger::Priority::DEBUG);
		log(format("\tAddress: %x %x\n", p->address.host, p->address.port), Logger::Priority::DEBUG);

		char *temp = (char *) p->data;
		
		parse_udp(p->len, temp);
	}
	
	
	if (!is_connected_)
		return;

	// Ping every second
	static Uint32 calculatedLag = SDL_GetTicks();
	Uint32 current = SDL_GetTicks();
	if (calculatedLag - current > 1000)
	{
		calculatedLag += 1000;
		CommandPing command;
		command.data.time = current;
		ServerClient::getInstance().send(command);
	}


	// Send update if ResetTimer was not reset for 200 milliseconds
	// To generally fix out of sync errors if they occur
	if (ServerClient::getInstance().getState() == ServerClient::State::INITIALIZED)
	{
		if ((SDL_GetTicks() - ServerClient::getInstance().getResetTimer()) > 200)
		{
			CommandSetPlayerData pos;
			try {
				player_util::set_position_data(pos, getClientId(), SDL_GetTicks(), ServerClient::getInstance().getUdpSeq(), player_util::get_player_by_id(getClientId()));
				ServerClient::getInstance().resetTimer();

				// Do not send update to server if we're dead
				if (!player_->is_dead && !ServerClient::getInstance().getGame().is_ended() && !ServerClient::getInstance().getGame().is_countdown())
					ServerClient::getInstance().send(pos);
			}
			catch (std::runtime_error &err) {
				log(err.what(), Logger::Priority::ERROR);
			}
		}
	}

	if (resumeGameTime_ && SDL_GetTicks() >= resumeGameTime_)
	{
		resumeGameTime_ = 0;

		game_->set_ended(false);
		if (resumeGameWithCountdown_)
			game_->set_countdown(true, 3 + 1);

		// Clear gameplay objects, powerups excluded, these are destroyed by command on the server
		std::vector<GameplayObject *> deleteObjs;
		for_each(begin(*game_->objects), end(*game_->objects), [&] (GameplayObject *obj) { 
			if (!obj->is_powerup)
				deleteObjs.push_back(obj); 
		});
		for_each(begin(deleteObjs), end(deleteObjs), [&] (GameplayObject *obj) { 
			auto &objects = (*game_->objects);
			auto iter = std::find(begin(objects), end(objects), obj);
			delete obj;
			if (iter != end(objects))
				objects.erase(iter);
		});
	}


	/* we poll keyboard every 1/10th of a second...simpler than threads */
	/* this is fine for a text application */
		
	/* wait on the socket for 1/10th of a second for data */
	int numready=SDLNet_CheckSockets(set, 0);
	if(numready==-1)
	{
		printf("SDLNet_CheckSockets: %s\n",SDLNet_GetError());
		return;
	}

	/* check to see if the server sent us data */
	if(numready && SDLNet_SocketReady(sock))
	{
		char buffer[16384] = {0x00};
		int bytesReceived = SDLNet_TCP_Recv(sock, buffer, sizeof(buffer));

		if (bytesReceived > 0)
		{
			receive(bytesReceived, buffer);
			while (parse())
					;
		}
		else
		{
			// Close the old socket, even if it's dead... 
			SDLNet_TCP_Close(sock);
				
			printf("Disconnected from server\n");
			is_connected_ = false;
			return;
		}
	}
}


void ServerClient::send(Command &command)
{
	if (!is_connected_)
		return;

	char type = command.getType();
	if (type == Command::Types::SetPlayerData ||
		type == Command::Types::Ping ||
		type == Command::Types::Pong
	){
		log(format("Send packet of type %d through UDP with seq %d", type, getUdpSeq()), Logger::Priority::CONSOLE);

		size_t packetsize = command.getDataLen() + sizeof (Uint32) + 1;
		
		char *packet = (char *)p->data;
		*packet = type;
		memcpy(packet + 1, (void *) &communicationToken_, sizeof (Uint32));
		memcpy(packet + 1 + sizeof (Uint32), command.getData(), command.getDataLen());

		p->data = (Uint8 *) packet;
		p->address.host = ip2.host;
		p->address.port = ip2.port;
		p->len = packetsize;

		int numsent = SDLNet_UDP_Send(sd, -1, p); /* This sets the p->channel */
		if(!numsent) {
			printf("SDLNet_UDP_Send^2: %s\n", SDLNet_GetError());
		}

		setNextUdpSeq();
	} else {
		log(format("Send packet of type %d through TCP", type), Logger::Priority::CONSOLE);
		result = SDLNet_TCP_Send(sock, &type, sizeof (char));

		if (result < sizeof (char)) {
			if (SDLNet_GetError() && strlen(SDLNet_GetError())) /* sometimes blank! */
				printf("SDLNet_TCP_Send: %s\n", SDLNet_GetError());
			return;
		}

		result = SDLNet_TCP_Send(sock, command.getData(), command.getDataLen());

		if (result < sizeof (char)) {
			if (SDLNet_GetError() && strlen(SDLNet_GetError())) /* sometimes blank! */
				printf("SDLNet_TCP_Send: %s\n", SDLNet_GetError());
			return;
		}
	}
	
}
void ServerClient::test()
{
	CommandPing command;
	command.data.time = SDL_GetTicks();

	this->send(command);
}

Gameplay &ServerClient::getGame()
{

	return *game_;
}


void ServerClient::resumeGameIn(short delay)
{
	resumeGameWithCountdown_ = delay != 0;

	delay -= static_cast<short>(lag.avg() + 0.5);
	if (delay < 0)
		delay = 0;

	// Substract lag
	resumeGameTime_ = SDL_GetTicks() + delay;

}


bool ServerClient::process(std::unique_ptr<Command> command)
{
	log(format("received packet of type: 0x%x", command.get()->getType()), Logger::Priority::CONSOLE);

	switch (command.get()->getType())
	{
		case Command::Types::Ping:
			process(dynamic_cast<CommandPing *>(command.get()));
			break;
		case Command::Types::Pong:
			process(dynamic_cast<CommandPong *>(command.get()));
			break;
		case Command::Types::SetLevel:
			process(dynamic_cast<CommandSetLevel *>(command.get()));
			break;
		case Command::Types::RequestCharacter:
			process(dynamic_cast<CommandRequestCharacter *>(command.get()));
			break;
		case Command::Types::SetPlayerData:
			process(dynamic_cast<CommandSetPlayerData *>(command.get()));
			break;
		case Command::Types::AddPlayer:
			process(dynamic_cast<CommandAddPlayer *>(command.get()));
			break;
		case Command::Types::DelPlayer:
			process(dynamic_cast<CommandDelPlayer *>(command.get()));
			break;
		case Command::Types::UpdateTile:
			process(dynamic_cast<CommandUpdateTile *>(command.get()));
			break;
		case Command::Types::ShotFired:
			process(dynamic_cast<CommandShotFired *>(command.get()));
			break;
		case Command::Types::BombDropped:
			process(dynamic_cast<CommandBombDropped *>(command.get()));
			break;
		case Command::Types::SetHitPoints:
			process(dynamic_cast<CommandSetHitPoints *>(command.get()));
			break;
		case Command::Types::SetPlayerAmmo:
			process(dynamic_cast<CommandSetPlayerAmmo *>(command.get()));
			break;
		case Command::Types::SetBroadcastText:
			process(dynamic_cast<CommandSetBroadcastText *>(command.get()));
			break;
		case Command::Types::SetPlayerDeath:
			process(dynamic_cast<CommandSetPlayerDeath *>(command.get()));
			break;
		case Command::Types::SetGameEnd:
			process(dynamic_cast<CommandSetGameEnd *>(command.get()));
			break;
		case Command::Types::SetPlayerScore:
			process(dynamic_cast<CommandSetPlayerScore *>(command.get()));
			break;
		case Command::Types::SetGameStart:
			process(dynamic_cast<CommandSetGameStart *>(command.get()));
			break;
		case Command::Types::GeneratePowerup:
			process(dynamic_cast<CommandGeneratePowerup *>(command.get()));
			break;
		case Command::Types::ApplyPowerup:
			process(dynamic_cast<CommandApplyPowerup *>(command.get()));
			break;
		case Command::Types::RemovePowerup:
			process(dynamic_cast<CommandRemovePowerup *>(command.get()));
			break;
		case Command::Types::SetCommunicationToken:
			process(dynamic_cast<CommandSetCommunicationToken *>(command.get()));
			break;
		case Command::Types::SetServerReady:
			process(dynamic_cast<CommandSetServerReady *>(command.get()));
			break;
		default:
			log(format("received command with type: %d", command.get()->getType()), Logger::Priority::CONSOLE);
	}

	return true;
}

bool ServerClient::process(CommandPing *command)
{
	CommandPong reply;
	reply.data.time = command->data.time;

	this->send(reply);

	log(format("PING? PONG!"), Logger::Priority::CONSOLE);

	return true;
}

bool ServerClient::process(CommandPong *command)
{
	float measuredLag = static_cast<float>((SDL_GetTicks() - command->data.time) / 2.0);
	
	lag.add(measuredLag);

	log(format("LAG %f AVG=%f", measuredLag, lag.avg()), Logger::Priority::CONSOLE);
	return true;
}


bool ServerClient::process(CommandSetLevel *command)
{
	my_id_ = command->data.your_id;

	// Default a client gets player with number zero
	player_->number = my_id_;
	
	player_->update_suit();
	player_->set_sprites();

	level_->load(level_util::get_filename_by_name(command->data.levelname).c_str());
	level_->reset();
	memcpy(&level_->level, &command->data.level, sizeof(level_->level));
	memcpy(&level_->level_hp, &command->data.level_hp, sizeof(level_->level_hp));
			
	game_->set_level(level_);
	
	log(format("my id is: %d load level %s!", command->data.your_id, command->data.level), Logger::Priority::CONSOLE);

	setState(ServerClient::State::INITIALIZED);

	return true;
}

bool ServerClient::process(CommandRequestCharacter *command)
{
	log(format("SERVER REQUESTED CHARACTER"), Logger::Priority::CONSOLE);

	CommandSetCharacter response;
	response.data.time = command->data.time;
	// response.data.nickname
	response.data.character = player_->character;

	this->send(response);

	return true;
}


bool ServerClient::process(CommandSetPlayerData *command)
{
	log(format("Server send player data"), Logger::Priority::DEBUG);

	if (my_id_ == command->data.client_id)
	{
		// Do not reset inputs!
		player_util::set_player_data(*player_, *command, true /* no resetting of inputs*/);
	}
	else
	{
		// You do receive the SetPlayerData command for each other player change
		auto &otherplayer = player_util::get_player_by_id(command->data.client_id);

		player_util::set_player_data(otherplayer, *command);

		// Account for lag
		int processFrames = static_cast<int>(lag.avg() / static_cast<float>(Main::MILLISECS_PER_FRAME));
		for (int i=0; i<processFrames; i++)
			game_->move_player(otherplayer);
	}

	return true;
}

bool ServerClient::process(CommandAddPlayer *command)
{
	Player *otherplayer = new Player(command->data.character, command->data.client_id);
	GameInputStub *playerinput = new GameInputStub();
	otherplayer->input = playerinput;

	otherplayer->position->x = command->data.x;
	otherplayer->position->y = command->data.y;

	// First reset, then set correct sprite (order is important)
	otherplayer->reset();
	otherplayer->set_sprite((command->data.current_sprite));
			
	game_->add_player(otherplayer);
	return true;
}

bool ServerClient::process(CommandDelPlayer *command)
{
	game_->del_player_by_id(command->data.client_id);
			
	return true;
}

bool ServerClient::process(CommandUpdateTile *command)
{
	level_->level_hp[command->data.tile] = command->data.tile_hp;

	if(level_->level_hp[command->data.tile] <= 0)
		level_->level[command->data.tile] = -1;

	return true;
}

bool ServerClient::process(CommandShotFired *command)
{
	auto &otherplayer = player_util::get_player_by_id(command->data.client_id);

	if (otherplayer.number == command->data.client_id) {
		// Make sure the correct sprite is set, so the bullet will go the correct direction
		otherplayer.current_sprite = command->data.current_sprite;

		Projectile *proj = otherplayer.create_projectile(command->data.x, command->data.y);

		proj->distance_traveled = command->data.distance_travelled;

		// Account for lag
		int processFrames = static_cast<int>(lag.avg() / static_cast<float>(Main::MILLISECS_PER_FRAME));
		for (int i=0; i<processFrames; i++)
			game_->process_gameplayobj(proj);
	}

	return true;
}

bool ServerClient::process(CommandBombDropped *command)
{
	// I'm currently too lazy to create function for it, I will refactor! (Todo!!)
	auto &players = *(game_->players);
	for (auto i=players.begin(); i!=players.end(); i++)
	{
		auto &otherplayer = **i;

		if (otherplayer.number == command->data.client_id)
		{
			// Make sure the correct sprite is set, so the bullet will go the correct direction
			otherplayer.current_sprite = command->data.current_sprite;

			GameplayObject *obj = otherplayer.create_bomb(command->data.x, command->data.y);

			// Account for lag
			int processFrames = static_cast<int>(lag.avg() / static_cast<float>(Main::MILLISECS_PER_FRAME));
			for (int i=0; i<processFrames; i++)
				game_->process_gameplayobj(obj);

			return true;
		}
	}
	return true;
}

bool ServerClient::process(CommandSetHitPoints *command)
{
	try {
		Player &player(player_util::get_player_by_id(command->data.client_id));

		log("Procesing hit point!", Logger::Priority::CONSOLE);
		player.hitpoints = command->data.hitpoints;
	}
	catch (std::runtime_error &err) {
		log(err.what(), Logger::Priority::CONSOLE);
	}

	return true;
}

bool ServerClient::process(CommandSetPlayerAmmo *command)
{
	try {
		Player &player(player_util::get_player_by_id(command->data.client_id));

		player.bombs = command->data.bombs;
	}
	catch (std::runtime_error &err) {
		log(err.what(), Logger::Priority::CONSOLE);
	}

	return true;
}

bool ServerClient::process(CommandSetBroadcastText *command)
{
	getGame().set_broadcast(command->data.text, command->data.duration);
	return true;
}

bool ServerClient::process(CommandSetPlayerDeath *command)
{
	try {
		Player &player(player_util::get_player_by_id(command->data.client_id));

		player.is_dead = command->data.is_dead;
	}
	catch (std::runtime_error &err) {
		log(err.what(), Logger::Priority::CONSOLE);
	}

	return true;
}

bool ServerClient::process(CommandSetGameEnd *command)
{
	try {
		Player &winner(player_util::get_player_by_id(command->data.winner_id));

		game_->set_ended(true);
		game_->set_draw(command->data.is_draw);
		game_->set_winner(winner);
	}
	catch (std::runtime_error &) {

		// Probably no winner, just a game-end.
		game_->set_ended(true);
		game_->set_draw(command->data.is_draw);
		game_->unset_winner();
	}

	return true;
}

bool ServerClient::process(CommandSetPlayerScore *command)
{
	try {
		Player &player(player_util::get_player_by_id(command->data.client_id));

		player.score = command->data.score;
	}
	catch (std::runtime_error &err) {
		log(err.what(), Logger::Priority::CONSOLE);
	}

	return true;
}

bool ServerClient::process(CommandSetGameStart *command)
{
	ServerClient::getInstance().resumeGameIn(command->data.delay);
	return true;
}

bool ServerClient::process(CommandGeneratePowerup *command)
{
	SDL_Rect *rect = new SDL_Rect;
	SDL_Rect *pos = new SDL_Rect;
	rect->w = 16;
	rect->h = 16;
	memcpy(pos, &command->data.position, sizeof(SDL_Rect));
	std::unique_ptr<GameplayObject> powerup = CommandGeneratePowerup::factory(command->data.type, command->data.powerupid, rect, pos, command->data.param);

	auto *newpowerup = powerup.release();

	game_->add_object(newpowerup);

	if (currentState_ != ServerClient::State::INITIALIZED)
		return true;

	// Account for lag
	int processFrames = static_cast<int>(lag.avg() / static_cast<float>(Main::MILLISECS_PER_FRAME));
	for (int i=0; i<processFrames; i++)
		game_->process_gameplayobj(newpowerup);

	return true;
}

bool ServerClient::process(CommandApplyPowerup *command)
{
	try {
		auto &objs = *game_->objects;
		for (auto i=objs.begin(); i!=objs.end(); i++) {
			auto &obj = **i;
			if (obj.is_powerup && obj.id() == command->data.powerup_id) {
				obj.hit_player(&player_util::get_player_by_id(command->data.player_id));
				obj.done = true;
				return true;
			}
		}
	}
	catch (std::runtime_error &err) 
	{
		log(format("failure in process(CommandApplyPowerup): %s", err.what()), Logger::Priority::ERROR);
	}

	return true;
}

bool ServerClient::process(CommandRemovePowerup *command)
{
	try {
		auto &objs = *game_->objects;
		for (auto i=objs.begin(); i!=objs.end(); i++) {
			auto *obj = *i;
			if (obj->is_powerup && obj->id() == command->data.powerup_id) {
				//obj.done = true;

				auto iter = std::find(begin(objs), end(objs), obj);
				delete obj;
				objs.erase(iter);

				return true;
			}
		}
	}
	catch (std::runtime_error &err) 
	{
		log(format("failure in process(CommandApplyPowerup): %s", err.what()), Logger::Priority::ERROR);
	}

	return true;
}

bool ServerClient::process(CommandSetCommunicationToken *command)
{
	try {
		communicationToken_ = command->data_.commToken;
		
		log(format("Acknowledging communication token from server"), Logger::Priority::CONSOLE);

		CommandCommunicationTokenAck response;
		response.data.time = command->data_.time;

		this->send(response);
		
	}
	catch (std::runtime_error &err) 
	{
		log(format("failure in process(CommandSetCommunicationToken): %s", err.what()), Logger::Priority::ERROR);
	}

	return true;
}

bool ServerClient::process(CommandSetServerReady *command)
{
	try {
		log(format("Confirming server ready with client ready"), Logger::Priority::CONSOLE);

		CommandSetClientReady response;

		this->send(response);
		
	}
	catch (std::runtime_error &err) 
	{
		log(format("failure in process(CommandSetServerReady): %s", err.what()), Logger::Priority::ERROR);
	}

	return true;
}

}