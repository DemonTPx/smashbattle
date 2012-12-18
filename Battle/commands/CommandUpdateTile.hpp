#ifndef __COMMAND_UPDATE_TILE__HPP__
#define __COMMAND_UPDATE_TILE__HPP__

#include "Command.hpp"

class CommandUpdateTile : public Command
{
public:

	CommandUpdateTile() : Command(Command::Types::UpdateTile) 
	{
		memset(&data, 0x00, sizeof(data));
	}
	~CommandUpdateTile() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		Uint32 time;
		int tile;
		int tile_hp;
	} data;
};


#endif //__COMMAND_UPDATE_TILE__HPP__