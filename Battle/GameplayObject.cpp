#include "SDL/SDL.h"

#include "Level.h"
#include "Player.h"
#include "GameplayObject.h"

GameplayObject::GameplayObject() {
	done = false;
	is_powerup = false;
}

GameplayObject::~GameplayObject() {
}
