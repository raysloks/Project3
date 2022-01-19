#pragma once

#include "CustomBehaviour.h"

#include <map>

class Mob;
class Window;
class SpriteSheet;

class HealthDisplay :
	public CustomBehaviour
{
public:
	void start();

	void tick(float dt);

	Reference<Mob> player;

	std::string format(int64_t number) const;
	std::string format(int64_t number, size_t right) const;

	Vec2 getAdvance(const std::string& text, const std::shared_ptr<SpriteSheet>& font) const;

private:
	std::shared_ptr<Window> window, bg, fg;

	std::vector<std::shared_ptr<Window>> text_windows;
	std::vector<std::shared_ptr<SpriteSheet>> fonts;

	std::string prev_text;
};

