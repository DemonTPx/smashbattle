#pragma once

#include "network/Command.hpp"

namespace network{

class CommandSetCommunicationToken : public Command
{
public:

	CommandSetCommunicationToken() : Command(Command::Types::SetCommunicationToken) 
	{
		memset(&data_, 0x00, sizeof(data_));
	}
	~CommandSetCommunicationToken() {	}

	virtual void * getData() { return &data_; };
	virtual size_t getDataLen() { return sizeof(data_); };

	struct
	{
		Uint32 time;
		Uint32 commToken;
	} data_;
};

}