#pragma once

#include "network/Command.hpp"

namespace network{

class CommandServerFull : public Command
{
public:

	CommandServerFull () : Command(Command::Types::ServerFull) { }
	~CommandServerFull() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		Uint32 time;
	} data;

private:
};

}