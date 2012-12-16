#ifndef __COMMANDPONG_HPP__
#define __COMMANDPONG_HPP__

#include "Command.hpp"

class CommandPong : public Command
{
public:

	CommandPong () : Command(Command::Types::Pong) { }
	~CommandPong() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	virtual void print() { std::cout << "pong: " << data.time << " !" << std::endl; };
		
	struct
	{
		Uint32 time;
	} data;


private:


};

#endif //__COMMANDPONG_HPP__