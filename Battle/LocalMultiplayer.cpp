#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"

#include "Main.h"
#include "Level.h"
#include "Gameplay.h"

#include "LocalMultiplayer.h"

void LocalMultiplayer::on_post_processing() {
	char ert[20];
	sprintf(ert, "%d", frame);
	SDL_WM_SetCaption(ert, NULL);
}
