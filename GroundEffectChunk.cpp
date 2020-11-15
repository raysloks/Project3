#include "GroundEffectChunk.h"

#include "Vec2.h"

#include <array>

#include <iostream>

#include <functional>

#include <SDL.h>

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

template <typename T>
constexpr T sqrt_helper(T x, T lo, T hi)
{
	if (lo == hi)
		return lo;

	const T mid = (lo + hi + 1) / 2;

	if (x / mid < mid)
		return sqrt_helper<T>(x, lo, mid - 1);
	else
		return sqrt_helper(x, mid, hi);
}

template <typename T>
constexpr T ct_sqrt(T x)
{
	return sqrt_helper<T>(x, 0, x / 2 + 1);
}

constexpr size_t amount(size_t x, size_t y)
{
	size_t shifted = (y >> (x * 2)) & 0b11;
	return shifted * shifted;
}

constexpr size_t crush(size_t x)
{
	return std::min(ct_sqrt(x), 0b11ull);
}

template <class Function>
constexpr auto make_array_type(Function func)
{
	std::array<std::array<size_t, 256>, 4> ret = {};
	for (size_t i = 0; i < 4; ++i)
		for (size_t j = 0; j < 256; ++j)
			ret[i][j] = func(i, j);
	return ret;
}

template <class Function>
constexpr auto make_array(Function func)
{
	std::array<std::invoke_result<Function, size_t>::type, 256> ret = {};
	for (size_t i = 0; i < 256; ++i)
		ret[i] = func(i);
	return ret;
}

constexpr auto lookup_type_amount = make_array_type(amount);
constexpr auto lookup_amount_crush = make_array([](size_t x) { return amount(0, crush(x)); });

void GroundEffectChunk::balance(size_t type, size_t& effect, const Vec2& p)
{
	if (!effects)
		effects = std::make_unique<uint8_t[]>(256);
	uint8_t& ground = at(p);
	auto ground_amount = lookup_type_amount[type][ground];
	auto total = effect + ground_amount;
	auto avg = total / 2;
	ground_amount = lookup_amount_crush[avg];
	effect = total - ground_amount;
	ground &= ~(0b11 << type);
	ground |= crush(ground_amount);
}

void GroundEffectChunk::deposit(size_t type, size_t & effect, const Vec2 & p)
{
	if (!effects)
		effects = std::make_unique<uint8_t[]>(256);
	uint8_t& ground = at(p);
	auto ground_amount = lookup_type_amount[type][ground];
	auto total = effect + ground_amount;
	ground_amount = lookup_amount_crush[total];
	effect = total - ground_amount;
	ground &= ~(0b11 << type);
	ground |= crush(ground_amount);
}

constexpr SDL_Color calc_color(uint8_t effect)
{
	SDL_Color color = {};

	auto&& r = color.r;
	auto&& g = color.g;
	auto&& b = color.b;
	auto&& a = color.a;

	size_t blood = lookup_type_amount[0][effect];
	size_t poison = lookup_type_amount[1][effect];
	size_t water = lookup_type_amount[2][effect];
	size_t oil = lookup_type_amount[3][effect];

	size_t total = blood + poison + water + oil;
	if (total > 0)
	{
		r += 100 * blood;
		r += 100 * poison;
		g += 200 * poison;
		g += 100 * water;
		b += 200 * water;

		r /= total;
		g /= total;
		b /= total;
	}

	a = blood;
	if (poison > a)
		a = poison;
	if (water > a)
		a = water;
	if (oil > a)
		a = oil;
	a *= 28;

	return color;
}

constexpr auto lookup_color = make_array(calc_color);

void GroundEffectChunk::color(size_t x, size_t y, SDL_Color& pixel) const
{
	pixel = lookup_color[at(x, y)];
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
