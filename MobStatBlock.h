#pragma once

#include <cstdint>

#include <array>

class MobStatBlock
{
public:
	MobStatBlock();

	union
	{
		struct
		{
			intmax_t hp;

			intmax_t strength;
			intmax_t agility;
			intmax_t intelligence;
		};
		std::array<intmax_t, 4> istat;
	};

	union
	{
		struct
		{
			float move_speed;
			float move_acc;
			float move_dec;
		};
		std::array<float, 4> fstat;
	};

	union
	{
		struct
		{
			float cc_multiplier;

			float attack_speed;

			float cooldown_multiplier;
		};
		std::array<float, 4> mstat;
	};
};

