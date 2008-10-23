#ifndef _TETRISTEST_H
#define _TETRISTEST_H

class TetrisTest {
private:
	TTF_Font * font28;
	SDL_Color fontColor;
	
	bool game_running;
public:
	TetrisTest();
	~TetrisTest();
	void run();
};

#endif