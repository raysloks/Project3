#include "GroundEffectChunk.h"

#include "Vec2.h"

#define GEC_WIDTH 16
#define GEC_HEIGHT 16

uint8_t GroundEffectChunk::get(const Vec2 & p) const
{
	if (effects)
		return at(p);
	return 0;
}

void GroundEffectChunk::set(const Vec2 & p, uint8_t effect)
{
	if (!effects)
		effects = std::make_unique<uint8_t[]>(256);
	at(p) = effect;
}

uint8_t GroundEffectChunk::get(size_t x, size_t y) const
{
	if (effects)
		return at(x, y);
	return 0;
}

void GroundEffectChunk::set(size_t x, size_t y, uint8_t effect)
{
	if (!effects)
		effects = std::make_unique<uint8_t[]>(256);
	at(x, y) = effect;
}

bool GroundEffectChunk::empty() const
{
	return !effects;
}

void GroundEffectChunk::clear()
{
	effects.reset();
}

uint8_t & GroundEffectChunk::at(size_t x, size_t y)
{
	return effects[x * GEC_WIDTH + y];
}

uint8_t & GroundEffectChunk::at(const Vec2 & p)
{
	return at(p.x, p.y);
}

const uint8_t & GroundEffectChunk::at(size_t x, size_t y) const
{
	return effects[x * GEC_WIDTH + y];
}

const uint8_t & GroundEffectChunk::at(const Vec2 & p) const
{
	return at(p.x, p.y);
}
