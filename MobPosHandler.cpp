#include "MobPosHandler.h"

#include "Player.h"

MobPosHandler::MobPosHandler()
{
	link.handler = this;
	link.Open(asio::ip::udp::endpoint(asio::ip::address_v6::any(), 0));

	link.Receive();
}

void MobPosHandler::MpChatHandler(const asio::ip::udp::endpoint & endpoint, const MpChat & message)
{
}

void MobPosHandler::MpGuidHandler(const asio::ip::udp::endpoint & endpoint, const MpGuid & message)
{
}

void MobPosHandler::MpMobUpdateHandler(const asio::ip::udp::endpoint & endpoint, const MpMobUpdate & message)
{
	if (message.id == player_mob_id)
		return;

	auto mob = mobs.find(message.id);
	if (mob != mobs.end())
	{
		mob->second.updates.push_back(message.data);
		//mob->second.mob->entity->xyz = message.position;
	}
	else
	{
		auto entity = level->add_entity();
		entity->xyz = message.data.position;

		auto sprite = level->sprites.add("bone_boy.png");
		Component::attach(sprite, entity);

		auto mob = level->add<NetworkMob>();
		Component::attach(mob, entity);

		MobPosData data;
		data.mob = mob;
		data.updates.push_back(message.data);

		mobs.emplace(std::make_pair(message.id, data));
	}
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

const float lower_margin = -4.0f;
const float nominal_margin_lower = -2.5f;
const float upper_margin = -1.0f;
const float nominal_margin_upper = -2.5f;

void MobPosData::tick(float dt)
{
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
