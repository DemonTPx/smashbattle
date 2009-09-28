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

const int Mission::MISSION_COUNT = 9;
const MISSION_INFO Mission::MISSIONS[Mission::MISSION_COUNT] = {
	{(char*)"INVASION OF THE CHICKS", (char*)"mission/001.lvl"},
	{(char*)"CHOP THE OFFICER", (char*)"mission/002.lvl"},
	{(char*)"KICK SOME ASS", (char*)"mission/003.lvl"},
	{(char*)"PUSH 'EM IN THE PIT", (char*)"mission/004.lvl"},
	{(char*)"BURN THEM WITH A JALAPENO", (char*)"mission/005.lvl"},
	{(char*)"STOMP HIS HEAD", (char*)"mission/006.lvl"},
	{(char*)"SUCKERPUNCHER", (char*)"mission/007.lvl"},
	{(char*)"POKE THE EYE OUT", (char*)"mission/008.lvl"},
	{(char*)"KILL THE FAGGOT", (char*)"mission/009.lvl"},
};

void Mission::initialize() {
	Gameplay::initialize();
}

void Mission::on_game_reset() {}

void Mission::on_pre_processing() {}

void Mission::on_post_processing() {}
