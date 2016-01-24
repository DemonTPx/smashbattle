#pragma once

#include "network/Command.hpp"

namespace network{

class CommandRequestCharacter : public Command
{
public:

	CommandRequestCharacter() : Command(Command::Types::RequestCharacter) { }
	~CommandRequestCharacter() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		Uint32 time;
	} data;
};

}