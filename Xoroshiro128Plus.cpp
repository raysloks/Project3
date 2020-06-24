#include "Xoroshiro128Plus.h"

uint64_t splitmix64(uint64_t& x) {
	uint64_t z = (x += UINT64_C(0x9E3779B97F4A7C15));
	z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
	z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
	return z ^ (z >> 31);
}

Xoroshiro128Plus::Xoroshiro128Plus()
{
	s[0] = 1;
	s[1] = 0;
}

Xoroshiro128Plus::Xoroshiro128Plus(uint64_t seed)
{
	s[0] = splitmix64(seed);
	s[1] = splitmix64(seed);
}

Xoroshiro128Plus::Xoroshiro128Plus(uint64_t seed0, uint64_t seed1)
{
	s[0] = seed0;
	s[1] = seed1;
}

void Xoroshiro128Plus::seed(uint64_t seed)
{
	s[0] = splitmix64(seed);
	s[1] = splitmix64(seed);
}

void Xoroshiro128Plus::seed(uint64_t seed0, uint64_t seed1)
{
	s[0] = seed0;
	s[1] = seed1;
}

uint64_t bitcount(uint64_t number)
{
	uint64_t i = 0;
	for (; number != 0; ++i)
		number >>= 1;
	return i;
}

uint64_t Xoroshiro128Plus::next(uint64_t max_inclusive)
{
	uint64_t shift = 64 - bitcount(max_inclusive);

	uint64_t number;
	do
	{
		number = next() >> shift;
	}
	while (number > max_inclusive);

	return number;
}

uint64_t Xoroshiro128Plus::next(uint64_t min_inclusive, uint64_t max_inclusive)
{
	return min_inclusive + next(max_inclusive - min_inclusive);
}

bool Xoroshiro128Plus::next_bool()
{
	return (int64_t)next() < 0;
}

float Xoroshiro128Plus::next_float()
{
	return (next() >> 40) * (1.0f / (1ull << 24));
}

/*  Written in 2016-2018 by David Blackman and Sebastiano Vigna (vigna@acm.org)

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.

See <http://creativecommons.org/publicdomain/zero/1.0/>. */

/* This is xoroshiro128+ 1.0, our best and fastest small-state generator
   for floating-point numbers. We suggest to use its upper bits for
   floating-point generation, as it is slightly faster than
   xoroshiro128**. It passes all tests we are aware of except for the four
   lower bits, which might fail linearity tests (and just those), so if
   low linear complexity is not considered an issue (as it is usually the
   case) it can be used to generate 64-bit outputs, too; moreover, this
   generator has a very mild Hamming-weight dependency making our test
   (http://prng.di.unimi.it/hwd.php) fail after 5 TB of output; we believe
   this slight bias cannot affect any application. If you are concerned,
   use xoroshiro128++, xoroshiro128** or xoshiro256+.

   We suggest to use a sign test to extract a random Boolean value, and
   right shifts to extract subsets of bits.

   The state must be seeded so that it is not everywhere zero. If you have
   a 64-bit seed, we suggest to seed a splitmix64 generator and use its
   output to fill s.

   NOTE: the parameters (a=24, b=16, b=37) of this version give slightly
   better results in our test than the 2016 version (a=55, b=14, c=36).
*/

static inline uint64_t rotl(const uint64_t x, int k) {
	return (x << k) | (x >> (64 - k));
}

uint64_t Xoroshiro128Plus::next(void) {
	const uint64_t s0 = s[0];
	uint64_t s1 = s[1];
	const uint64_t result = s0 + s1;

	s1 ^= s0;
	s[0] = rotl(s0, 24) ^ s1 ^ (s1 << 16); // a, b
	s[1] = rotl(s1, 37); // c

	return result;
}


/* This is the jump function for the generator. It is equivalent
   to 2^64 calls to next(); it can be used to generate 2^64
   non-overlapping subsequences for parallel computations. */

void Xoroshiro128Plus::jump(void) {
	static const uint64_t JUMP[] = { 0xdf900294d8f554a5, 0x170865df4b3201fc };

	uint64_t s0 = 0;
	uint64_t s1 = 0;
	for (int i = 0; i < sizeof JUMP / sizeof * JUMP; i++)
		for (int b = 0; b < 64; b++) {
			if (JUMP[i] & UINT64_C(1) << b) {
				s0 ^= s[0];
				s1 ^= s[1];
			}
			next();
		}

	s[0] = s0;
	s[1] = s1;
}


/* This is the long-jump function for the generator. It is equivalent to
   2^96 calls to next(); it can be used to generate 2^32 starting points,
   from each of which jump() will generate 2^32 non-overlapping
   subsequences for parallel distributed computations. */

void Xoroshiro128Plus::long_jump(void) {
	static const uint64_t LONG_JUMP[] = { 0xd2a98b26625eee7b, 0xdddf9b1090aa7ac1 };

	uint64_t s0 = 0;
	uint64_t s1 = 0;
	for (int i = 0; i < sizeof LONG_JUMP / sizeof * LONG_JUMP; i++)
		for (int b = 0; b < 64; b++) {
			if (LONG_JUMP[i] & UINT64_C(1) << b) {
				s0 ^= s[0];
				s1 ^= s[1];
			}
			next();
		}

	s[0] = s0;
	s[1] = s1;
}
