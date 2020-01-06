#include "Tile.h"

uint8_t * Tile::operator[](size_t x)
{
	return &effects[x * 16];
}
