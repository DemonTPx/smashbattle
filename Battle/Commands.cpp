#include "SDL/SDL.h"

#include "Commands.hpp"

std::unique_ptr<Command> Command::factory(Command::Types type)
{
	// implicit move
	switch (type)
	{
		case Command::Types::Ping:
			return std::unique_ptr<Command>(new CommandPing());
		case Command::Types::Pong:
			return std::unique_ptr<Command>(new CommandPong());
		case Command::Types::Position:
			return std::unique_ptr<Command>(new CommandSetPosition());
		case Command::Types::SetLevel:
			return std::unique_ptr<Command>(new CommandSetLevel());
		case Command::Types::RequestCharacter:
			return std::unique_ptr<Command>(new CommandRequestCharacter());
		case Command::Types::SetCharacter:
			return std::unique_ptr<Command>(new CommandSetCharacter());
		case Command::Types::SetPlayerData:
			return std::unique_ptr<Command>(new CommandSetPlayerData());
		case Command::Types::AddPlayer:
			return std::unique_ptr<Command>(new CommandAddPlayer());
		case Command::Types::DelPlayer:
			return std::unique_ptr<Command>(new CommandDelPlayer());
		case Command::Types::UpdateTile:
			return std::unique_ptr<Command>(new CommandUpdateTile());
		case Command::Types::ShotFired:
			return std::unique_ptr<Command>(new CommandShotFired());
		case Command::Types::BombDropped:
			return std::unique_ptr<Command>(new CommandBombDropped());
	}
	throw std::runtime_error("failure");
}