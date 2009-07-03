#include "SDL/SDL.h"

#include "Player.h"
#include "GameplayObject.h"

GameplayObject::GameplayObject() {
	done = false;
}

void GameplayObject::move() {
	// Default = static object, doesn't move
}

void GameplayObject::process() {
	// Default = static object, doesn't do anything
}

void GameplayObject::hit_player(Player * player) {
	// Default = static object, doesn't do anything
}

void GameplayObject::draw(SDL_Surface * screen) {
	// TODO: draw a placeholder perhaps?
}
