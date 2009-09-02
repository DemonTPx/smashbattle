#include "SDL/SDL.h"

#include "Main.h"
#include "Level.h"
#include "Gameplay.h"

#include "NPC.h"
#include "ChickNPC.h"

#include "Mission.h"

#ifndef WIN32
#define sprintf_s snprintf
#endif

void Mission::initialize() {
	Gameplay::initialize();
}

void Mission::on_game_reset() {}

void Mission::on_pre_processing() {}

void Mission::on_post_processing() {}
