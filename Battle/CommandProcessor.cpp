#include "SDL/SDL.h"

#include <memory>

// temp
#include "Client.h"

#include "Commands.hpp"

void CommandProcessor::receive(int bytes_received, const char * const buffer)
{
	memcpy(buffer_ + buffer_idx_,
					buffer,
					bytes_received);
					
	buffer_idx_ += bytes_received;
}

#include "log.h"
bool CommandProcessor::parse()
{
	size_t processed = 0;

	if (expectRequestFor_) {

		try  {
			auto cmd = Command::factory(static_cast<Command::Types>(expectRequestFor_));
			void *request = cmd.get()->getData();
			size_t requestLen = cmd.get()->getDataLen();

			if ((size_t)buffer_idx_ >= requestLen) {
				memcpy(request, buffer_, requestLen);

				cmd.get()->print();

				log(format("Received packet of type %d", expectRequestFor_), Logger::Priority::DEBUG);
				process(std::move(cmd));

				expectRequestFor_ = 0;
				processed += requestLen;
			}

			
		}
		catch (std::runtime_error &err)
		{
			std::cout << " error: " << err.what() << std::endl;
		}
	}
	else if (buffer_idx_ >= sizeof(char)) 
	{
		expectRequestFor_ = *buffer_;

		processed += sizeof(char);
	}
	
	if (!processed)
		return false;

	memcpy(buffer_, buffer_ +  processed, buffer_idx_ - processed);
	buffer_idx_ -= processed;

	return true;

}
