#pragma once

#include "network/Command.hpp"

class CommandSetCommunicationToken : public Command
{
public:

	CommandSetCommunicationToken() : Command(Command::Types::SetCommunicationToken) 
	{
		memset(&data, 0x00, sizeof(data));
	}
	~CommandSetCommunicationToken() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		Uint32 time;
		uint64_t commToken;
	} data;
};