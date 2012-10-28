/**
 * The SDL_Net code could not have been so easily written without the examples provided by
 *  Jon C. Atkins, found here: http://jcatki.no-ip.org:8080/SDL_net/
 */

#include "ServerClient.h"

ServerClient::ServerClient()
	: is_connected_(false)
{
}

ServerClient::~ServerClient()
{
	SDLNet_Quit();
}

void ServerClient::connect()
{
	if (is_connected_)
		return;

	/* initialize SDL */
	if(SDL_Init(0)==-1)
	{
		printf("SDL_Init: %s\n",SDL_GetError());
		return;
	}

	/* initialize SDL_net */
	if(SDLNet_Init()==-1)
	{
		printf("SDLNet_Init: %s\n",SDLNet_GetError());
		return;
	}

	set=SDLNet_AllocSocketSet(1);
	if(!set)
	{
		printf("SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
		return;
	}

	/* get the port from the commandline */
	port=(Uint16)1099;
	
	/* Resolve the argument into an IPaddress type */
	printf("Connecting to %s port %d\n","localhost",port);
	if(SDLNet_ResolveHost(&ip,"localhost",port)==-1)
	{
		printf("SDLNet_ResolveHost: %s\n",SDLNet_GetError());
		return;
	}

	/* open the server socket */
	sock=SDLNet_TCP_Open(&ip);
	if(!sock)
	{
		printf("SDLNet_TCP_Open: %s\n",SDLNet_GetError());
		return;
	}
	
	if(SDLNet_TCP_AddSocket(set,sock)==-1)
	{
		printf("SDLNet_TCP_AddSocket: %s\n",SDLNet_GetError());
		return;
	}
	
	printf("Ready\n");
	is_connected_ = true;
}

void ServerClient::poll(short test)
{
	if (!is_connected_)
		return;

	/* we poll keyboard every 1/10th of a second...simpler than threads */
	/* this is fine for a text application */
		
	/* wait on the socket for 1/10th of a second for data */
	int numready=SDLNet_CheckSockets(set, 0);
	if(numready==-1)
	{
		printf("SDLNet_CheckSockets: %s\n",SDLNet_GetError());
		return;
	}

	/* check to see if the server sent us data */
	if(numready && SDLNet_SocketReady(sock))
	{
		char buffer[512] = {0x00};
		int bytesReceived = SDLNet_TCP_Recv(sock, buffer, sizeof(buffer));

		if (bytesReceived > 0)
		{
			printf("@todo impl@received bytes from server.\n");
			//bytesReceived, buffer
		}
		else
		{
			// Close the old socket, even if it's dead... 
			SDLNet_TCP_Close(sock);
				
			printf("Disconnected from server\n");
			is_connected_ = false;
			return;
		}

	}
	
	// test string to server (once)

	result=SDLNet_TCP_Send(sock, (const void *)&test,sizeof(short));
	if(result<sizeof(short)) {
		if(SDLNet_GetError() && strlen(SDLNet_GetError())) /* sometimes blank! */
			printf("SDLNet_TCP_Send: %s\n", SDLNet_GetError());
		return;
	}
}