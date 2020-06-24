#pragma once

#include <cstdint>

class Xoroshiro128Plus
{
public:
	Xoroshiro128Plus();
	Xoroshiro128Plus(uint64_t seed);
	Xoroshiro128Plus(uint64_t seed0, uint64_t seed1);

	void seed(uint64_t seed);
	void seed(uint64_t seed0, uint64_t seed1);

	uint64_t next(uint64_t max_inclusive);
	uint64_t next(uint64_t min_inclusive, uint64_t max_inclusive);

	bool next_bool();

	// in the range [0, 1)
	float next_float();

	uint64_t next();
	void jump();
	void long_jump();

private:
	uint64_t s[2];
};

