#define CATCH_CONFIG_MAIN
#include "catch.hpp"

class ServerState
{
public:
	ServerState() {}
	virtual const std::string type() const = 0;
};
class DerivedServerState : public ServerState
{
public:
	const std::string type() const { return typeid(this).name(); }
};

TEST_CASE("ServerState::type() includes the classname in string")
{
	DerivedServerState state;
	REQUIRE(state.type().find("DerivedServerState") != std::string::npos);
	REQUIRE(state.type().find("XerivedServerState") == std::string::npos);
}