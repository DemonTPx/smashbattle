#pragma once

class Main;

class SimpleDrawable
{
public:

	void draw();

protected:

	SimpleDrawable(Main &main);

	virtual void draw_impl() = 0;

	Main &main_;
};