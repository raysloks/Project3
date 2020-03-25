#include "MobStatBlock.h"

MobStatBlock::MobStatBlock()
{
	for (auto& stat : istat)
		stat = 0;

	for (auto& stat : fstat)
		stat = 0.0f;

	for (auto& stat : mstat)
		stat = 1.0f;
}
