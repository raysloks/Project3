#pragma once

#include <cstdint>

class Xoroshiro128Plus
{
public:
	Xoroshiro128Plus();

	void seed(uint64_t seed);

	// max_exclusive must be larger than 0
	uint64_t next(uint64_t max_exclusive);

	// in the range [0, 1)
	float next_float();

	uint64_t next();
	void jump();
	void long_jump();

private:
	uint64_t s[2];
};

