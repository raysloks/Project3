#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <memory>
#include <iostream>

class MpMobAurasUpdate;
class MpMobCastUpdate;
class MpMobHealthUpdate;
class MpMobStateUpdate;
class MpMobTypeUpdate;
class MpPath;

#pragma pack(push, 1)
class MpMobUpdate
{
public:
	uint64_t id;
	std::unique_ptr<MpMobHealthUpdate> hp;
	std::unique_ptr<MpPath> path;
	std::unique_ptr<MpMobAurasUpdate> auras;
	std::unique_ptr<MpMobStateUpdate> state;
	std::unique_ptr<MpMobTypeUpdate> type;
	std::unique_ptr<MpMobCastUpdate> cast;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
#pragma pack(pop)
