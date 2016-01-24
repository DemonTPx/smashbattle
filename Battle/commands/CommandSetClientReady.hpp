#pragma once

#include "network/Command.hpp"

namespace network{

class CommandSetClientReady : public Command
{
public:

	CommandSetClientReady () : Command(Command::Types::SetClientReady) { }
	~CommandSetClientReady() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
	} data;

private:
};

}