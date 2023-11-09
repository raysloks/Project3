#pragma once

#include "Window.h"

class Font;
class TextArea;

class NamePlate :
	public Window
{
public:
	NamePlate();

	virtual void setName(const std::string& name);
	virtual void setHealth(int health);
	virtual void setMaxHealth(int max_health);

private:
	std::weak_ptr<Window> bg, fg;
	std::vector<std::weak_ptr<TextArea>> text_windows;

	int health_current, health_max;
	std::string last_text;

	void updateHealthText();
};

