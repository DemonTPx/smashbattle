#ifndef _PROJECTILE_H
#define _PROJECTILE_H

class Projectile {
public:
	Projectile();
	Projectile(SDL_Surface * surface, SDL_Rect * clip);
	~Projectile();

	SDL_Rect * position;
	SDL_Surface * sprite;
	SDL_Rect * clip;

	int speedx, speedy;
	int distance_traveled;
	int max_distance;
	bool hit;
	
	void show(SDL_Surface * screen);
private:
};

#endif
