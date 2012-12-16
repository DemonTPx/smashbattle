#ifndef __COMMAND_ADD_PLAYER__HPP__
#define __COMMAND_ADD_PLAYER__HPP__

#include "Command.hpp"

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
		bool facingRight;

		// Health et.al.

	} data;
};


#endif //__COMMAND_ADD_PLAYER__HPP__