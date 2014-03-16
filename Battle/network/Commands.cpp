#include "SDL/SDL.h"

#include "network/Commands.hpp"
#include <stdexcept>

#include "util/Log.h"

namespace network {

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
		case Command::Types::SetHitPoints:
			return std::unique_ptr<Command>(new CommandSetHitPoints());
		case Command::Types::SetPlayerAmmo:
			return std::unique_ptr<Command>(new CommandSetPlayerAmmo());
		case Command::Types::SetBroadcastText:
			return std::unique_ptr<Command>(new CommandSetBroadcastText());
		case Command::Types::SetPlayerDeath:
			return std::unique_ptr<Command>(new CommandSetPlayerDeath());
		case Command::Types::SetGameEnd:
			return std::unique_ptr<Command>(new CommandSetGameEnd());
		case Command::Types::SetPlayerScore:
			return std::unique_ptr<Command>(new CommandSetPlayerScore());
		case Command::Types::SetGameStart:
			return std::unique_ptr<Command>(new CommandSetGameStart());
		case Command::Types::GeneratePowerup:
			return std::unique_ptr<Command>(new CommandGeneratePowerup());
		case Command::Types::ApplyPowerup:
			return std::unique_ptr<Command>(new CommandApplyPowerup());
		case Command::Types::RemovePowerup:
			return std::unique_ptr<Command>(new CommandRemovePowerup());
		case Command::Types::SetCommunicationToken:
			return std::unique_ptr<Command>(new CommandSetCommunicationToken());
		case Command::Types::CommunicationTokenAck:
			return std::unique_ptr<Command>(new CommandCommunicationTokenAck());
		case Command::Types::SetServerReady:
			return std::unique_ptr<Command>(new CommandSetServerReady());
		case Command::Types::SetClientReady:
			return std::unique_ptr<Command>(new CommandSetClientReady());
		case Command::Types::SetSpectating:
			return std::unique_ptr<Command>(new CommandSetSpectating());
		case Command::Types::ApiPing:
			return std::unique_ptr<Command>(new CommandApiPing());
		case Command::Types::ApiPong:
			return std::unique_ptr<Command>(new CommandApiPong());
		case Command::Types::ServerFull:
			return std::unique_ptr<Command>(new CommandServerFull());

	}
	throw std::runtime_error(format("failure at type=%d 0x%x", type, type));
}

}