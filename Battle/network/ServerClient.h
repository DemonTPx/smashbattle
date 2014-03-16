#pragma once

#include <string>

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


#include "network/CommandProcessor.h"
#include "util/LagMeasure.hpp"

class Level;
class Player;
class Gameplay;
class Main;

namespace network{

class ClientNetworkMultiplayer;

class Command;
class CommandPing ;
class CommandPong;
class CommandSetLevel;
class CommandRequestCharacter;
class CommandSetCharacter;
class CommandSetPlayerData;
class CommandAddPlayer;
class CommandDelPlayer;
class CommandUpdateTile;
class CommandShotFired;
class CommandBombDropped;
class CommandSetHitPoints;
class CommandSetPlayerAmmo;
class CommandSetBroadcastText;
class CommandSetPlayerDeath;
class CommandSetGameEnd;
class CommandSetPlayerScore;
class CommandSetGameStart;
class CommandGeneratePowerup;
class CommandApplyPowerup;
class CommandRemovePowerup;
class CommandSetCommunicationToken;
class CommandSetServerReady;
class CommandSetSpectating;
class CommandServerFull;

class ServerClient : public CommandProcessor
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
	

	enum State
	{
		INITIALIZING = 1,
		INITIALIZED = 2
	};

	
	ServerClient();

	void setMain(Main &main) { main_ = &main; };

	void setCharacter(int character) { character_ = character; }
	int characterByName(std::string characterName);
	void setHost(std::string host) { host_ = host; }
	void setPort(int port) { port_ = (Uint16)port; }

	void connect(ClientNetworkMultiplayer &game, Level &level, Player &player, Main &main);
	void disconnect();
	void poll();

	void test();
	void send(Command &command);

	bool isConnected() { return is_connected_; }
	bool isCommTokenAvailable() { return communicationToken_ != 0; }
	void toggleConsole() { show_console_ = !show_console_ ; }
	bool showConsole() { return show_console_; }

	void setState(ServerClient::State state) { currentState_ = state; }
	ServerClient::State getState() { return currentState_; }

	LagMeasure &getLag() { return lag; }
	Gameplay &getGame();

	char getClientId() { return my_id_; }
	int getCharacter() { return character_; }

	void resetTimer() { lastResetTimer_ = SDL_GetTicks(); }
	Uint32 getResetTimer() { return lastResetTimer_; }

	void resumeGameIn(short delay);


protected:
	bool process(std::unique_ptr<Command> command);
	
	bool process(CommandPing *command);
	bool process(CommandPong *command);
	bool process(CommandSetLevel *command);
	bool process(CommandRequestCharacter *command);
	bool process(CommandSetPlayerData *command);
	bool process(CommandAddPlayer *command);
	bool process(CommandDelPlayer *command);
	bool process(CommandUpdateTile *command);
	bool process(CommandShotFired *command);
	bool process(CommandBombDropped *command);
	bool process(CommandSetHitPoints *command);
	bool process(CommandSetPlayerAmmo *command);
	bool process(CommandSetBroadcastText *command);
	bool process(CommandSetPlayerDeath *command);
	bool process(CommandSetGameEnd *command);
	bool process(CommandSetPlayerScore *command);
	bool process(CommandSetGameStart *command);
	bool process(CommandGeneratePowerup *command);
	bool process(CommandApplyPowerup *command);
	bool process(CommandRemovePowerup *command);
	bool process(CommandSetCommunicationToken *command);
	bool process(CommandSetServerReady *command);
	bool process(CommandSetSpectating *command);
	bool process(CommandServerFull *command);

	
private:
	~ServerClient();
	
	ServerClient(ServerClient const&); // Don't implement
	void operator=(ServerClient const&); // Don't implement

	bool is_connected_;
	bool show_console_;

	
	ServerClient::State currentState_;

	int character_;
	IPaddress ip;
	IPaddress ip2;
	TCPsocket sock;
	std::string host_;
	Uint16 port_;
	SDLNet_SocketSet set;
	fd_set fdset;
	size_t result;
	char *name,*str;
	struct timeval tv;

	// received
	char buffer_[8192];
	int buffer_idx_;

	// send buffer
	char send_buffer_[8192];
	int send_buffer_idx_;

	LagMeasure lag;

	char my_id_;

	ClientNetworkMultiplayer * game_;
	Level * level_;
	Player * player_;

	Uint32 lastResetTimer_;
	bool resumeGameWithCountdown_;
	Uint32 resumeGameTime_;
	
	UDPsocket sd;
	UDPpacket *p;

	Main *main_;
};

}