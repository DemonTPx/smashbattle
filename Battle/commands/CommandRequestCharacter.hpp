#ifndef __COMMAND_REQUEST_CHARACTER__HPP__
#define __COMMAND_REQUEST_CHARACTER__HPP__

#include "Command.hpp"

class CommandRequestCharacter : public Command
{
public:

	CommandRequestCharacter() : Command(Command::Types::RequestCharacter) { }
	~CommandRequestCharacter() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		Uint32 time;
	} data;
};


#endif //__COMMAND_REQUEST_CHARACTER__HPP__