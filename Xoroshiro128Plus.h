#pragma once

#include <cstdint>

class Xoroshiro128Plus
{
public:
	Xoroshiro128Plus();

	void seed(uint64_t seed);

	uint64_t next(uint64_t max_inclusive);
	uint64_t next(uint64_t min_inclusive, uint64_t max_inclusive);

	// in the range [0, 1)
	float next_float();

	uint64_t next();
	void jump();
	void long_jump();

private:
	uint64_t s[2];
};

