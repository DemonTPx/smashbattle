#ifndef __COMMAND_SET_GAME_START__HPP__
#define __COMMAND_SET_GAME_START__HPP__

#include "Command.hpp"

class CommandSetGameStart : public Command
{
public:

	CommandSetGameStart() : Command(Command::Types::SetGameStart) 
	{
		memset(&data, 0x00, sizeof(data));
	}
	~CommandSetGameStart() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		Uint32 time;
		short delay;
	} data;
};


#endif //__COMMAND_SET_GAME_START__HPP__