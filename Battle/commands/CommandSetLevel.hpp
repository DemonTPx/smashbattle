#ifndef __COMMANDSETLEVEL_HPP__
#define __COMMANDSETLEVEL_HPP__

#include "Command.hpp"

class CommandSetLevel : public Command
{
public:

	CommandSetLevel () : Command(Command::Types::SetLevel) 
	{
		memset(&data, 0x00, sizeof(data));
	}
	~CommandSetLevel() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		char your_id;
		char level[80 + 1];
	} data;

private:
};

#endif //__COMMANDSETLEVEL_HPP__