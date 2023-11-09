#include "MpMobUpdate.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpMobUpdate::serialize(std::ostream& os) const
{
	os.write((char*)&this->id, sizeof(this->id));
	if (this->hp)
	{
		os.put(true);
		auto&& value = *this->hp;
	value.serialize(os);
	}
	else
	{
		os.put(false);
	}
	if (this->path)
	{
		os.put(true);
		auto&& value = *this->path;
	value.serialize(os);
	}
	else
	{
		os.put(false);
	}
	if (this->auras)
	{
		os.put(true);
		auto&& value = *this->auras;
	value.serialize(os);
	}
	else
	{
		os.put(false);
	}
	if (this->state)
	{
		os.put(true);
		os.write((char*)this->state.get(), sizeof(MpMobStateUpdate));
	}
	else
	{
		os.put(false);
	}
	if (this->type)
	{
		os.put(true);
		os.write((char*)this->type.get(), sizeof(MpMobTypeUpdate));
	}
	else
	{
		os.put(false);
	}
	if (this->cast)
	{
		os.put(true);
		auto&& value = *this->cast;
	value.serialize(os);
	}
	else
	{
		os.put(false);
	}
}

void MpMobUpdate::deserialize(std::istream& is)
{
	is.read((char*)&this->id, sizeof(this->id));
	if (is.get())
	{
		this->hp = std::make_unique<MpMobHealthUpdate>();
		auto&& value = *this->hp;
	value.deserialize(is);
	}
	if (is.get())
	{
		this->path = std::make_unique<MpPath>();
		auto&& value = *this->path;
	value.deserialize(is);
	}
	if (is.get())
	{
		this->auras = std::make_unique<MpMobAurasUpdate>();
		auto&& value = *this->auras;
	value.deserialize(is);
	}
	if (is.get())
	{
		this->state = std::make_unique<MpMobStateUpdate>();
		is.read((char*)this->state.get(), sizeof(MpMobStateUpdate));
	}
	if (is.get())
	{
		this->type = std::make_unique<MpMobTypeUpdate>();
		is.read((char*)this->type.get(), sizeof(MpMobTypeUpdate));
	}
	if (is.get())
	{
		this->cast = std::make_unique<MpMobCastUpdate>();
		auto&& value = *this->cast;
	value.deserialize(is);
	}
}

