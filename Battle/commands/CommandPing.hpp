#ifndef __COMMANDPING_HPP__
#define __COMMANDPING_HPP__

#include "Command.hpp"

class CommandPing : public Command
{
public:

	CommandPing () : Command(Command::Types::Ping) { }
	~CommandPing() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		Uint32 time;
	} data;

private:
};

#endif //__COMMANDPING_HPP__