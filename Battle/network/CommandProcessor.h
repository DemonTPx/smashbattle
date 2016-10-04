#pragma once

#include <memory>
#include "SDL2/SDL.h"
#include "SDL2/SDL_net.h"

namespace network{

class Command;

class CommandProcessor
{
protected:

	CommandProcessor(TCPsocket socket)
		: socket_(socket),
		  buffer_idx_(0),
		  expectRequestFor_(0),
		  communicationToken_(0),
		  udpsequence_(0)
	{
		memset(buffer_, 0x00, sizeof(buffer_));
	}
	CommandProcessor() : buffer_idx_(0) 
	{
		memset(buffer_, 0x00, sizeof(buffer_));
	}
	virtual ~CommandProcessor() {}

	void set_socket(TCPsocket socket) { socket_ = socket;}

public:

	void parse_udp(int bytes_received, const char * const buffer);
	void receive(int bytes_received, const char * const buffer);
	bool parse();
	short getUdpSeq() { return udpsequence_; }
	void setNextUdpSeq() { udpsequence_++; }

	
protected:
	
	virtual bool process(std::unique_ptr<Command> command) = 0;

	
	TCPsocket socket_;
	// received
	char buffer_[32768];
	int buffer_idx_;
	char expectRequestFor_;

	// UDP
	Uint32 communicationToken_;
	short udpsequence_;
};

}
