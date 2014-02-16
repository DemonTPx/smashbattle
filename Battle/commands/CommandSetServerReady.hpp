#pragma once

#include "network/Command.hpp"

class CommandSetServerReady : public Command
{
public:

	CommandSetServerReady () : Command(Command::Types::SetServerReady) { }
	~CommandSetServerReady() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
	} data;

private:
};