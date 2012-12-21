#ifndef __COMMAND_HPP__
#define __COMMAND_HPP__

#include <memory>
#include <iostream>

class Command
{
public:

	enum Types 
	{
		Ping = 0x01,
		Pong = 0x02,
		Position = 0x03,
		SetLevel = 0x04,
		RequestCharacter = 0x05,
		SetCharacter = 0x06,
		SetPlayerData = 0x07,
		AddPlayer = 0x08,
		DelPlayer = 0x09,
		UpdateTile = 0x0A,
		ShotFired = 0x0B,
		BombDropped = 0x0C,
		SetHitPoints = 0x0D,
		SetPlayerAmmo = 0x0E,
		SetBroadcastText = 0x0F,
		SetPlayerDeath = 0x10,
		SetGameEnd = 0x11,
		SetPlayerScore = 0x12,
		SetGameStart = 0x13
	};

	Command::Types getType() const { return type_; }
	virtual void * getData()  = 0;
	virtual size_t getDataLen() = 0;


	Command(Command::Types type) : type_(type) {}
	~Command() {}

	static std::unique_ptr<Command> factory(Command::Types type);

	virtual void print() {};

protected:

	Command::Types type_;

private:
};
#endif //__COMMAND_HPP__