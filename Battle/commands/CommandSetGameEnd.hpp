#pragma once

#include "Command.hpp"

class CommandSetGameEnd : public Command
{
public:

	CommandSetGameEnd() : Command(Command::Types::SetGameEnd) 
	{
		memset(&data, 0x00, sizeof(data));
	}
	~CommandSetGameEnd() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		Uint32 time;
		char winner_id;
		bool is_draw;
	} data;
};