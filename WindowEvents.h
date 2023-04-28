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

class KeyEvent :
	public WindowEvent
{
public:
	uint64_t key;
	uint64_t mod;
};

class KeyDownEvent :
	public KeyEvent
{
public:
	bool repeat;
};

class KeyUpEvent :
	public KeyEvent
{
public:
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

class LayoutEvent :
	public WindowEvent
{
public:
};
