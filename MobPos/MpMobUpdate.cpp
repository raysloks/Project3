#include "MpMobUpdate.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

#include "MpMobHealthUpdate.h"
#include "MpMobStateUpdate.h"
#include "MpPath.h"

void MpMobUpdate::serialize(std::ostream& os) const
{
	os.write((char*)&id, (sizeof(id) + 3) / 4 * 4);
	if (hp)
	{
		os.put(true);
		this->hp->serialize(os);
	}
	else
	{
		os.put(false);
	}
	if (path)
	{
		os.put(true);
		this->path->serialize(os);
	}
	else
	{
		os.put(false);
	}
	if (stats)
	{
		os.put(true);
		os.write((char*)this->stats.get(), sizeof(MobStatBlock));
	}
	else
	{
		os.put(false);
	}
	if (state)
	{
		os.put(true);
		os.write((char*)this->state.get(), sizeof(MpMobStateUpdate));
	}
	else
	{
		os.put(false);
	}
}

void MpMobUpdate::deserialize(std::istream& is)
{
	is.read((char*)&id, (sizeof(id) + 3) / 4 * 4);
	if (is.get())
	{
		hp = std::make_unique<MpMobHealthUpdate>();
		this->hp->deserialize(is);
	}
	if (is.get())
	{
		path = std::make_unique<MpPath>();
		this->path->deserialize(is);
	}
	if (is.get())
	{
		stats = std::make_unique<MobStatBlock>();
		is.read((char*)this->stats.get(), sizeof(MobStatBlock));
	}
	if (is.get())
	{
		state = std::make_unique<MpMobStateUpdate>();
		is.read((char*)this->state.get(), sizeof(MpMobStateUpdate));
	}
}

