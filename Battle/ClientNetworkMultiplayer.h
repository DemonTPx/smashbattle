#ifndef __CLIENTNETWORKMULTIPLAYER_H__
#define __CLIENTNETWORKMULTIPLAYER_H__

#include "LocalMultiplayer.h"

class ClientNetworkMultiplayer : public LocalMultiplayer 
{
public:
	ClientNetworkMultiplayer() {}
	~ClientNetworkMultiplayer() {}

protected:

	virtual void on_game_reset();

	virtual void on_pre_processing() {}
	virtual void on_post_processing() {}

	virtual void on_input_handled();

};

#endif //__CLIENTNETWORKMULTIPLAYER_H__