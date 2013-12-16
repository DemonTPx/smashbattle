#pragma once

#include <memory>
#include "Command.hpp"

#include "GameplayObject.h"

struct SDL_Rect;

class CommandGeneratePowerup : public Command
{
public:

	enum PowerUps
	{
		TypeHealth = 0x01,
		TypeAmmo = 0x02,
		TypeDoubleDamage = 0x03,
		TypeInstantKillBullet = 0x04,
		TypeBomb = 0x05,
		TypeMine = 0x06,
		TypeAirstrike = 0x07,
		TypeLaserBeam = 0x08,
		TypeShield = 0x09,
		TypeRandom = 0x0A
	};

	static std::unique_ptr<GameplayObject> factory(
		CommandGeneratePowerup::PowerUps type,
		short powerupid,
		SDL_Rect *rect,
		SDL_Rect *pos,
		int param = 0
	);

	CommandGeneratePowerup() : Command(Command::Types::GeneratePowerup) 
	{
		memset(&data, 0x00, sizeof(data));
	}
	~CommandGeneratePowerup() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		Uint32 time;
		short powerupid;
		CommandGeneratePowerup::PowerUps type;
		SDL_Rect position;
		int param;
	} data;
};