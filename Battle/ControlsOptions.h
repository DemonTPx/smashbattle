#ifndef _CONTROLSOPTIONS_H
#define _CONTROLSOPTIONS_H
#include "OptionsScreen.h"

class ControlsOptions : public OptionsScreen {
public:
	ControlsOptions(ControlScheme * controls);

	void run();
	void item_selected();
private:
	ControlScheme * controls;
	ControlScheme new_controls;
	bool keyboard_redefined;
	bool joystick_redefined;
	bool joystick_idx_changed;

	void redefine_keyboard();
	int poll_keyboard(const char * question);
	void redefine_joystick();
	int poll_joystick(int index, const char * question);
	void show_notification(const char * text);
};

class JoystickSelect : public OptionsScreen {
public:
	JoystickSelect(int index);

	void item_selected();

	int index;
private:
	int cancel_idx;
};

#endif
