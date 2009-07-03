#ifndef _GAMEPLAYOBJECT_H
#define _GAMEPLAYOBJECT_H

class GameplayObject {
public:
	GameplayObject();
	
	virtual void move();
	virtual void process();

	virtual void hit_player(Player * player);
	//virtual void hit_npc(NPC * npc);

	virtual void draw(SDL_Surface * screen);

	bool done;

	SDL_Rect * position;
};

#endif
