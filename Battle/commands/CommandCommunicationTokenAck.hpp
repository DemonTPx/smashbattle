#pragma once

#include "network/Command.hpp"

class CommandCommunicationTokenAck : public Command
{
public:

	CommandCommunicationTokenAck() : Command(Command::Types::CommunicationTokenAck) 
	{
		memset(&data, 0x00, sizeof(data));
	}
	~CommandCommunicationTokenAck() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		Uint32 time;
	} data;
};