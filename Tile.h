#pragma once

#include <cstdint>

class Tile
{
public:
	uint8_t * operator[](size_t x);
	
	uint32_t tile;
	uint8_t * effects;
};

