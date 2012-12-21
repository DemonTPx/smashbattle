#ifndef __NETWORKMULTIPLAYER_H__
#define __NETWORKMULTIPLAYER_H__

#include "LocalMultiplayer.h"

class NetworkMultiplayer : public LocalMultiplayer 
{
public:
	NetworkMultiplayer () {}
	~NetworkMultiplayer() {}

protected:

	virtual void on_game_reset();

	virtual void on_pre_processing() {}
	virtual void on_post_processing();

};

#endif //__NETWORKMULTIPLAYER_H__
