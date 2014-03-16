#pragma once

#include "OptionsScreen.h"

class PlayerAnimation;
struct OptionItem;

class ClientSettings : public OptionsScreen {
public:
	ClientSettings(Main &main);
	virtual ~ClientSettings();

	void initialize();

	void item_selected();

	virtual void on_pre_draw();
	virtual void on_post_draw();

protected:

private:
	int character_;

	std::string text;

	PlayerAnimation *anim;

	void create_items();

	void connect();
	void show_error(const std::string &error_msg);

	std::string serverToken_;
	std::string selectServerText_;

	Main &main_;
};