#pragma once

#include "network/Command.hpp"

namespace network{

class CommandApiPong : public Command
{
public:

	CommandApiPong () : Command(Command::Types::ApiPong) { }
	~CommandApiPong() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	virtual void print() { /* std::cout << "pong: " << data.time << " !" << std::endl; */ };
		
	struct
	{
		Uint32 time;
		char num_joined_players;
		char num_active_players;
	} data;


private:


};

}