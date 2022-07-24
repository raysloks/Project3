#pragma once

#include <cstdint>
#include <string>

#include "Vec2.h"

class WindowEvent
{
public:
	Vec2 mouse_position;
};

class KeyDownEvent :
	public WindowEvent
{
public:
	uint64_t key;
};

class KeyUpEvent :
	public WindowEvent
{
public:
	uint64_t key;
};

class TextInputEvent :
	public WindowEvent
{
public:
	std::string text;
};
