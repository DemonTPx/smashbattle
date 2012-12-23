#ifndef __COMMAND_APPLY_POWERUP__HPP__
#define __COMMAND_APPLY_POWERUP__HPP__

#include "Command.hpp"

class CommandApplyPowerup : public Command
{
public:

	CommandApplyPowerup() : Command(Command::Types::ApplyPowerup) 
	{
		memset(&data, 0x00, sizeof(data));
	}
	~CommandApplyPowerup() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		Uint32 time;
		short powerup_id;
		char player_id;
	} data;
};


#endif //__COMMAND_APPLY_POWERUP__HPP__