#pragma once

#include "network/Command.hpp"

namespace network {

class CommandSetPlayerScore : public Command
{
public:

	CommandSetPlayerScore() : Command(Command::Types::SetPlayerScore) 
	{
		memset(&data, 0x00, sizeof(data));
	}
	~CommandSetPlayerScore() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		Uint32 time;
		char client_id;
		int score;
	} data;
};

}