#pragma once

#include <vector>

#define GAMEINPUT_TYPE_KEY		0x0001
#define GAMEINPUT_TYPE_JBUTTON	0x0002
#define GAMEINPUT_TYPE_JAXIS	0x0003
#define GAMEINPUT_TYPE_JHAT		0x0004

#define ACTION_COUNT 10

#define A_LEFT	0
#define A_RIGHT	1
#define A_UP	2
#define A_DOWN	3
#define A_JUMP	4
#define A_RUN	5
#define A_SHOOT	6
#define A_BOMB	7
#define A_START	8
#define A_BACK	9

struct GameInputKeyBind {
	int key;
	int action;
};

struct GameInputJoyButtonBind {
	int button;
	int action;
};

struct GameInputJoyAxisBind {
	int axis;
	int threshold;
	int action;
};

struct GameInputJoyHatBind {
	int hat;
	int direction;
	int action;
};

struct GameInputJoystickEvent {
	enum {
		NONE,
		BUTTON,
		AXIS,
		HAT
	} type;

	int button;
	
	int axis_idx;
	int axis_value;

	int hat_idx;
	int hat_direction;
};

namespace network {
	class Client;
	class ClientNetworkMultiplayer;
}

class GameInput {
public:
	GameInput(Main &main);
	virtual ~GameInput();

	GameInput * clone(bool clone_binds = true);
	void copy_from(GameInput * gi);

	bool open_joystick(int index);
	int get_joystick_idx();

	int num_buttons();
	int num_axes();
	int num_hats();

	void enable_keyboard(bool enable);
	void enable_joystick(bool enable);

	void bind_key(int key, int action);
	void bind_joybutton(int button, int action);
	void bind_joyaxis(int axis, bool positive, int action);
	void bind_joyaxis(int axis, int threshold, int action);
	void bind_joyhat(int hat, int direction, int action);

	int get_joyaxis(int axis);

	virtual void handle_event(SDL_Event * event);

	bool is_pressed(int action);

	void reset();

	void set_delay(int delay = 18, int interval = 6);
	void unset_delay();

	int keyboard_wait_event();
	void joystick_wait_event(GameInputJoystickEvent * event);

	void keyboard_wait_event_bind(int action);
	void joystick_wait_event_bind(int action);

	void load_options(std::istream * stream, short version);
	void save_options(std::ostream * stream);

	void flush_keybinds();
	void flush_joybuttons();
	void flush_joyaxes();
	void flush_joyhats();

	bool keyboard_enabled;
	bool joystick_enabled;

protected:
	SDL_Joystick * joystick;
	int joystick_idx;

	std::vector<GameInputKeyBind> * keybinds;
	std::vector<GameInputJoyButtonBind> * joybuttonbinds;
	std::vector<GameInputJoyAxisBind> * joyaxisbinds;
	std::vector<GameInputJoyHatBind> * joyhatbinds;

	int delay;
	int interval;

public:
	bool pressed[ACTION_COUNT];
protected:
	unsigned int press_start[ACTION_COUNT];
	short press_owner[ACTION_COUNT];

	void joystick_wait_released();

	static const int JOYSTICK_AXIS_THRESHOLD;

	// temporary for accessing pressed array :p
	friend class Gameplay;
	friend class network::Client;
	friend class network::ClientNetworkMultiplayer;

	Main &main_;
};


class GameInputStub : public GameInput
{
public:
	GameInputStub(Main &main) : GameInput(main) { }

	virtual void handle_event(SDL_Event * event) { /* handle nothing */ }
};
