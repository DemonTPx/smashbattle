#pragma once

#include "OptionsScreen.h"

class PlayerAnimation;
class OptionItem;

class ClientSettings : public OptionsScreen
{
public:
	ClientSettings();
	virtual ~ClientSettings();
	
	void initialize();
	
	void run();
	void item_selected();
	
	virtual void on_pre_draw();
	virtual void on_post_draw();
	
protected:
	
	virtual bool process_event(SDL_Event &event);
	virtual void process_cursor();
	
private:

	std::string nickname_;
	int character_;
	
	OptionItem *oitem1_;
	OptionItem *oitem2_;
	OptionItem *oitem3_;
	
	std::string item1_;
	std::string item2_;
	std::string item3_;
	
	PlayerAnimation *anim;
	
	bool editting_nickname_;


	void connect();
	void show_error(const std::string &error_msg);
	
	std::string serverToken_;
};