#pragma once

#include "Window.h"

class SpriteSheet;

class NineSlice :
	public Window
{
public:
	NineSlice(const std::shared_ptr<SpriteSheet>& sheet);
};

