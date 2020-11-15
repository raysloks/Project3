#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <memory>
#include <iostream>

#include "MobStatBlock.h"

class MpMobHealthUpdate;
class MpMobStateUpdate;
class MpPath;

#pragma pack(push, 1)
class MpMobUpdate
{
public:
	uint64_t id;
	std::unique_ptr<MpMobHealthUpdate> hp;
	std::unique_ptr<MpPath> path;
	std::unique_ptr<MobStatBlock> stats;
	std::unique_ptr<MpMobStateUpdate> state;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
#pragma pack(pop)
