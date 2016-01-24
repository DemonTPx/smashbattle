#pragma once

#include "network/Command.hpp"

namespace network{

class CommandPong : public Command
{
public:

	CommandPong () : Command(Command::Types::Pong) { }
	~CommandPong() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	virtual void print() { /* std::cout << "pong: " << data.time << " !" << std::endl; */ };
		
	struct
	{
		Uint32 time;
	} data;


private:


};

}