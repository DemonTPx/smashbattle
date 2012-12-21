#ifndef __COMMAND_SET_BROADCAST_TEXT_HPP__
#define __COMMAND_SET_BROADCAST_TEXT_HPP__

#include "Command.hpp"

class CommandSetBroadcastText : public Command
{
public:

	CommandSetBroadcastText() : Command(Command::Types::SetBroadcastText) 
	{
		memset(&data, 0x00, sizeof(data));
	}
	~CommandSetBroadcastText() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		Uint32 time;

		char text[80+1];
		Uint32 duration;

	} data;
};


#endif //__COMMAND_SET_BROADCAST_TEXT_HPP__