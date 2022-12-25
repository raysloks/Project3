#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

#include "Vec2.h"

class Window;

class WindowEvent
{
public:
	Vec2 cursor_position;
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

class CursorMoveEvent :
	public WindowEvent
{
public:
	uint64_t cursor_index;
};

class CursorEnterEvent :
	public CursorMoveEvent
{
public:
};

class CursorLeaveEvent :
	public CursorMoveEvent
{
public:
};

class FocusGainedEvent :
	public WindowEvent
{
public:
};

class FocusLostEvent :
	public WindowEvent
{
public:
};
