#pragma once

#include "network/Command.hpp"

namespace network{

class CommandApiPing : public Command
{
public:

	CommandApiPing () : Command(Command::Types::ApiPing) { }
	~CommandApiPing() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		Uint32 time;
	} data;

private:
};

}