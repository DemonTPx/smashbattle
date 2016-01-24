#pragma once

#include "network/Command.hpp"

namespace network {

class CommandSetPlayerDeath : public Command
{
public:

	CommandSetPlayerDeath() : Command(Command::Types::SetPlayerDeath) 
	{
		memset(&data, 0x00, sizeof(data));
	}
	~CommandSetPlayerDeath() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		Uint32 time;
		char client_id;
		bool is_dead;
	} data;
};

}