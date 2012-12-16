#ifndef __COMMAND_SET_CHARACTER__HPP__
#define __COMMAND_SET_CHARACTER__HPP__

#include "Command.hpp"

class CommandSetCharacter : public Command
{
public:

	CommandSetCharacter() : Command(Command::Types::SetCharacter) 
	{
		memset(&data, 0x00, sizeof(data));
	}
	~CommandSetCharacter() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		Uint32 time;
		char character;
		char nickname[80 + 1];
	} data;
};


#endif //__COMMAND_SET_CHARACTER__HPP__