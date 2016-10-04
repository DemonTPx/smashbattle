#include "SDL2/SDL.h"

#include <memory>
#include <sstream>
#include <functional>

// temp
#include "network/Client.h"

#include "network/Commands.hpp"
#include "util/Log.h"

namespace network {

void CommandProcessor::parse_udp(int bytes_received, const char * const buffer)
{
	if (bytes_received == 0) {
		throw std::runtime_error("cannot receive zero bytes");
	}
	char expectRequestForBackup = expectRequestFor_;
	
	try {
		std::stringstream ss;
		ss << format("parse_udp[%d, %d] = ", buffer_idx_, bytes_received);
		for (int i=0; i<bytes_received; i++) {
			char p[2] = {0x00};
			p[0] = buffer[i];
			ss << format(" %x", *p);
		}
		ss << std::endl;
		log(ss.str(), Logger::Priority::DEBUG);
		
		// The expected packet..
		expectRequestFor_ = buffer[0];
		
		auto cmd = Command::factory(static_cast<Command::Types>(expectRequestFor_));
		size_t requestLen = cmd.get()->getDataLen();
		if (requestLen != (bytes_received - 1))
			throw std::runtime_error("unexpected data len for given packet");
		
		// Insert the UDP packet data in front (exclude the packet type indicator (the -1))
		memmove(buffer_ + bytes_received - 1, buffer_, buffer_idx_);
		buffer_idx_ += bytes_received - 1;

		// Insert the packet in front of the queue
		memcpy(buffer_, buffer + 1, bytes_received - 1);

		// Parse the data
		parse();
		expectRequestFor_ = expectRequestForBackup;
	}
	catch (...) {
		expectRequestFor_ = expectRequestForBackup;
		throw;
	}
}

void CommandProcessor::receive(int bytes_received, const char * const buffer)
{
	std::stringstream ss;
	ss << format("receive[%d, %d] = ", buffer_idx_, bytes_received);
	for (int i=0; i<bytes_received; i++) {
		char p[2] = {0x00};
		p[0] = buffer[i];
		ss << format(" %x", *p);
	}
	ss << std::endl;
	log(ss.str(), Logger::Priority::DEBUG);
	
	memcpy(buffer_ + buffer_idx_,
					buffer,
					bytes_received);
					
	buffer_idx_ += bytes_received;
}

bool CommandProcessor::parse()
{
	size_t processed = 0;

	std::unique_ptr<Command> cmd = nullptr;
	std::function<void()> processCommand = nullptr;

	if (expectRequestFor_) {

		try {
			cmd = Command::factory(static_cast<Command::Types>(expectRequestFor_));
			void *request = cmd.get()->getData();
			size_t requestLen = cmd.get()->getDataLen();

			if ((size_t)buffer_idx_ >= requestLen) {
				std::stringstream ss;
				ss << format("processing[%d, %d] = ", buffer_idx_, requestLen);
				for (int i=0; i<requestLen; i++) {
					char p[2] = {0x00};
					p[0] = *(buffer_ + i);
					ss << format(" %x", *p);
				}
				ss << std::endl;
				log(ss.str(), Logger::Priority::DEBUG);
				
				memcpy(request, buffer_, requestLen);

				cmd.get()->print();

				log(format("Received packet of type 0x%x of length %d", expectRequestFor_, requestLen), Logger::Priority::DEBUG);
				processCommand = [&]() {
					log(format("..processing.."), Logger::Priority::DEBUG);
					process(std::move(cmd));
				};

				expectRequestFor_ = 0;
				processed += requestLen;
			}

			
		}
		catch (std::runtime_error &err)
		{
			std::cout << " error: " << err.what() << std::endl;
		}
	}
	else if (static_cast<size_t>(buffer_idx_) >= sizeof(char)) 
	{
		expectRequestFor_ = *buffer_;
		char p[2] = {0x00};
		p[0] = *buffer_;
		log(format("processing[1] = %x (expectRequestFor is set to this)\n", *p), Logger::Priority::DEBUG);
		
		processed += sizeof(char);
	}
	
	if (!processed)
		return false;

	// This gives undefined behaviour in Linux :)
	// Windows it was no problem, so I guess the implementation is different..
	//memcpy(buffer_, buffer_ +  processed, buffer_idx_ - processed);
	memmove(buffer_, buffer_ + processed, buffer_idx_ - processed);

	buffer_idx_ -= processed;

	if (processCommand != nullptr)
		processCommand();

	return true;

}

}
