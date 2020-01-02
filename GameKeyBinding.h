#pragma once

#include <cstdint>

enum GameKeyBinding : uint64_t
{
	_KB_BASE = (1ull << 32ull),
	KB_ATTACK,
	KB_UP,
	KB_LEFT,
	KB_DOWN,
	KB_RIGHT
};
