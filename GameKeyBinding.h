#pragma once

#include <cstdint>

enum GameKeyBinding : uint64_t
{
	_KB_BASE = (1ull << 32ull),
	KB_UP,
	KB_LEFT,
	KB_DOWN,
	KB_RIGHT,
	KB_ACTION_0,
	KB_ACTION_1,
	KB_ACTION_2,
	KB_ACTION_3,
	KB_ACTION_4,
	KB_ACTION_5,
	KB_ACTION_6,
	KB_ACTION_7,
	KB_ACTION_8,
	KB_ACTION_9
};
