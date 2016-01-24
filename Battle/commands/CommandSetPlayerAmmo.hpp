#pragma once

#include "network/Command.hpp"

namespace network {

class CommandSetPlayerAmmo : public Command
{
public:

	CommandSetPlayerAmmo() : Command(Command::Types::SetPlayerAmmo) 
	{
		memset(&data, 0x00, sizeof(data));
	}
	~CommandSetPlayerAmmo() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		Uint32 time;
		char client_id;

		int bombs;

	} data;
};

}