#include "MobPosHandler.h"

#include "Player.h"

#include <iostream>

MobPosHandler::MobPosHandler()
{
	link.handler = this;
	link.Open(asio::ip::udp::endpoint(asio::ip::address_v6::any(), 0));

	link.Receive();
}

void MobPosHandler::MpAttackHandler(const asio::ip::udp::endpoint & endpoint, const MpAttack & message)
{
}

void MobPosHandler::MpChatHandler(const asio::ip::udp::endpoint & endpoint, const MpChat & message)
{
}

void MobPosHandler::MpDamageHandler(const asio::ip::udp::endpoint & endpoint, const MpDamage & message)
{
	auto mob = mobs.find(message.id);
	if (mob != mobs.end())
	{
		mob->second.mob->onDamaged(message.damage);
	}
}

void MobPosHandler::MpGuidHandler(const asio::ip::udp::endpoint & endpoint, const MpGuid & message)
{
}

std::vector<std::string> temp = { "bone_boy.png", "imp.png" };

void MobPosHandler::MpMobSpriteUpdateHandler(const asio::ip::udp::endpoint & endpoint, const MpMobSpriteUpdate & message)
{
	if (message.id == player_mob_id)
		return;

	auto mob = getMob(message.id);
	mob->second.mob->getComponent<Sprite>()->sheet = SpriteSheet::get(temp[message.sprite]);
}

void MobPosHandler::MpMobUpdateHandler(const asio::ip::udp::endpoint & endpoint, const MpMobUpdate & message)
{
	if (message.id == player_mob_id)
		return;

	auto mob = getMob(message.id);
	mob->second.updates.push_back(message.data);
}

void MobPosHandler::MpMobUpdateDataHandler(const asio::ip::udp::endpoint & endpoint, const MpMobUpdateData & message)
{
}

void MobPosHandler::MpPlayerMobCreatedHandler(const asio::ip::udp::endpoint & endpoint, const MpPlayerMobCreated & message)
{
	player_mob_id = message.id;
}

void MobPosHandler::MpSoundHandler(const asio::ip::udp::endpoint & endpoint, const MpSound & message)
{
}

void MobPosHandler::MpUpdateHandler(const asio::ip::udp::endpoint & endpoint, const MpUpdate & message)
{
}

void MobPosHandler::tick(float dt)
{
	for (auto& i : mobs)
	{
		i.second.tick(dt);
	}

	auto player = level->get<Player>();
	if (player)
	{
		MpMobUpdate message;
		message.data.position = player->entity->xyz;
		message.data.velocity = player->v;
		message.data.facing = player->move;
		link.Send(asio::ip::udp::endpoint(asio::ip::address::from_string("2001:2002:51eb:2940:f30b:983c:2892:83c2"), 43857), message);
	}
}

std::map<uint64_t, MobPosData>::iterator MobPosHandler::getMob(uint64_t id)
{
	auto mob = mobs.find(id);
	if (mob != mobs.end())
		return mob;
	createMob(id);
	return mobs.find(id);
}

void MobPosHandler::createMob(uint64_t id)
{
	auto entity = level->add_entity();

	auto sprite = level->sprites.add("uu.png");
	Component::attach(sprite, entity);

	auto mob = level->add<NetworkMob>();
	Component::attach(mob, entity);

	auto mob_entity = entity;

	// create shadow
	{
		auto entity = level->add_entity();
		Entity::adopt(entity, mob_entity);

		auto sprite = level->sprites.add("shadow4_iso.png");
		sprite->sort = 0;
		sprite->color = SDL_Color({ 0, 0, 0, 32 });
		Component::attach(sprite, entity);
	}

	MobPosData data;
	data.mob = mob;

	mobs.emplace(std::make_pair(id, data));
}

const float lower_margin = -4.0f;
const float nominal_margin_lower = -2.5f;
const float upper_margin = -1.0f;
const float nominal_margin_upper = -2.5f;

void MobPosData::tick(float dt)
{
	if (updates.empty())
		return;

	accumulator += dt * 20.0f;

	if (accumulator > updates.size() + upper_margin)
		accumulator = updates.size() + nominal_margin_upper;
	if (accumulator < updates.size() + lower_margin)
		accumulator = updates.size() + nominal_margin_lower;

	int lower = std::floorf(accumulator);
	int upper = std::ceilf(accumulator);
	float frac = accumulator - lower;

	if (lower < 0)
		lower = 0;
	if (upper < 0)
		upper = 0;

	if (lower > updates.size() - 1)
		lower = updates.size() - 1;
	if (upper > updates.size() - 1)
		upper = updates.size() - 1;

	mob->entity->xyz = updates[lower].position * (1.0f - frac) + updates[upper].position * frac;
	mob->v = updates[lower].velocity * (1.0f - frac) + updates[upper].velocity * frac;
	mob->move = updates[lower].facing * (1.0f - frac) + updates[upper].facing * frac;

	if (lower == upper)
		lower -= 1;

	lower -= 2;

	if (lower > 0)
	{
		updates.erase(updates.begin(), updates.begin() + lower);
		accumulator -= lower;
	}
}
