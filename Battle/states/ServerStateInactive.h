#ifndef __INCLUDE__SERVERSTATEINACTIVE_H__
#define __INCLUDE__SERVERSTATEINACTIVE_H__

#include "ServerState.h"

class ServerStateInactive : public ServerState
{

public:
	ServerStateInactive();

	const std::string type() const { return typeid(this).name(); }
};


#endif //__INCLUDE__SERVERSTATEINACTIVE_H__