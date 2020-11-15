#pragma once

#include <cstdint>

#include <memory>

class Vec2;
struct SDL_Color;

class GroundEffectChunk
{
public:
	uint8_t get(const Vec2& p) const;
	void set(const Vec2& p, uint8_t effect);

	uint8_t get(size_t x, size_t y) const;
	void set(size_t x, size_t y, uint8_t effect);

	bool empty() const;
	void clear();

	bool dirty;

	void balance(size_t type, size_t& effect, const Vec2& p);
	void deposit(size_t type, size_t& effect, const Vec2& p);

	void color(size_t x, size_t y, SDL_Color& pixel) const;

private:
	uint8_t & at(size_t x, size_t y);
	uint8_t & at(const Vec2& p);
	const uint8_t & at(size_t x, size_t y) const;
	const uint8_t & at(const Vec2& p) const;

	std::unique_ptr<uint8_t[]> effects;
};

