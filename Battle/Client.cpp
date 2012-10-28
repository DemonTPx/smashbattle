#include "Client.h"

Client::Client(int client_id, TCPsocket socket)
	: client_id_(client_id),
	  socket_(socket),
	  buffer_idx_(0),
	  test(0)
{
	memset(buffer_, 0x00, sizeof(buffer_));
}


void Client::receive(int bytes_received, const char * const buffer)
{
	memcpy(buffer_ + buffer_idx_,
					buffer,
					bytes_received);
					
	buffer_idx_ += bytes_received;
	
	printf("Client received: %s\n", buffer);
	short *test2 = (short *)buffer_;
	printf("short recvd: %d\n", *test2);

	test = *test2;

	buffer_idx_ = 0;// temp hack
	// Todo: Process here!! Now buffer_ will overflow  :)
}
