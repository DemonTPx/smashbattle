#ifndef _PROJECTILE_H
#define _PROJECTILE_H

class Projectile {
public:
	Projectile();
	Projectile(const char * sprite_file, SDL_Rect * clip);
	~Projectile();

	SDL_Rect * position;
	SDL_Surface * sprite;
	SDL_Rect * clip;

	int speedx, speedy;
	int distance_traveled;
	int max_distance;
	bool hit;
	
	void show(SDL_Surface * screen);

	void load_images(const char * sprite_file);
private:
	void free_images();
};

#endif
