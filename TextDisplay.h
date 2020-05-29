#pragma once

#include "CustomBehaviour.h"

class TextDisplay :
	public CustomBehaviour
{
public:
	void start();

	void setText(const std::string& text);

	std::vector<Reference<Sprite>> sprites;

	std::string text;

	std::shared_ptr<SpriteSheet> font;
private:
	void updateSprites();

	Reference<Sprite> getSprite(size_t index);
};

