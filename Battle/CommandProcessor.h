#ifndef __COMMAND_PROCESSOR_H___
#define __COMMAND_PROCESSOR_H___

#include <memory>
#include "SDL/SDL.h"
#include "SDL/SDL_net.h"

class Command;

class CommandProcessor
{
protected:

	CommandProcessor(TCPsocket socket)
		: socket_(socket),
		  buffer_idx_(0),
		  expectRequestFor_(0)
	{
		memset(buffer_, 0x00, sizeof(buffer_));
	}
	CommandProcessor() : buffer_idx_(0) 
	{
		memset(buffer_, 0x00, sizeof(buffer_));
	}
	~CommandProcessor() {}

	void set_socket(TCPsocket socket) { socket_ = socket;}

public:

	void receive(int bytes_received, const char * const buffer);
	bool parse();
	virtual bool process(std::unique_ptr<Command> command) = 0;

	TCPsocket socket_;
	// received
	char buffer_[8192];
	int buffer_idx_;
	char expectRequestFor_;

	
};

#endif //__COMMAND_PROCESSOR_H___