#pragma once

#include "network/Command.hpp"

namespace network{

/**
 * \Brief Command to notify the connected client that he is spectating.
 * 
 * For other players there is a flag "is_spectating" in the CommandAddPlayer command.
 */
class CommandSetSpectating : public Command
{
public:

	CommandSetSpectating () : Command(Command::Types::SetSpectating) { }
	~CommandSetSpectating() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		Uint32 time;
		bool is_spectating;
	} data;

private:
};

}