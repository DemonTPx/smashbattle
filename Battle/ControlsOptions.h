#pragma once

#include "OptionsScreen.h"

class ControlsOptions : public OptionsScreen {
public:
	ControlsOptions(GameInput * input, int number);
	~ControlsOptions();

	void run();
	void item_selected();
protected:
	virtual void process_cursor();
private:
	GameInput * input;
	GameInput * new_input;

	void redefine_keyboard();
	void redefine_joystick();

	void poll_keyboard(int action, const char * question);
	void poll_joystick(int action, const char * question);
	void show_notification(const char * text);

	bool save_permitted();
	void save_input();
};

class JoystickSelect : public OptionsScreen {
public:
	JoystickSelect(int index);

	void item_selected();

	int index;
private:
	int cancel_idx;
	char joystick_name[10][80];
};
