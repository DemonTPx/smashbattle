#pragma once

#include "network/Command.hpp"

class CommandAddPlayer : public Command
{
public:

	CommandAddPlayer() : Command(Command::Types::AddPlayer) 
	{
		memset(&data, 0x00, sizeof(data));
	}
	~CommandAddPlayer() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		Uint32 time;
		char client_id;
		char character;

		Sint16 x;
		Sint16 y;
		int current_sprite;

		// Health et.al.

	} data;
};