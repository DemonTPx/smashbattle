#pragma once

#include "network/Command.hpp"

namespace network {

class CommandSetPosition : public Command
{
public:

	CommandSetPosition () : Command(Command::Types::Position) { }
	~CommandSetPosition() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	virtual void print() { std::cout << "pos: " << data.time << " en " << data.flags << std::endl; };

	struct
	{
		Uint32 time;
		short flags;
		Sint16 x;
		Sint16 y;
	} data;
};

}