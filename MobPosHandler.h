#pragma once

#include "System.h"

#include "MobPos/MpLink.h"

#include "Engine.h"

#include "NetworkMob.h"

class MobPosData
{
public:
	Reference<NetworkMob> mob;
	std::vector<MpMobUpdateData> updates;
	float accumulator;

	void tick(float dt);
};

class MobPosHandler :
	public System
{
public:
	MobPosHandler();

	void MpAttackHandler(const asio::ip::udp::endpoint& endpoint, const MpAttack& message);
	void MpChatHandler(const asio::ip::udp::endpoint& endpoint, const MpChat& message);
	void MpDamageHandler(const asio::ip::udp::endpoint& endpoint, const MpDamage& message);
	void MpGuidHandler(const asio::ip::udp::endpoint& endpoint, const MpGuid& message);
	void MpMobSpriteUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobSpriteUpdate& message);
	void MpMobUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobUpdate& message);
	void MpMobUpdateDataHandler(const asio::ip::udp::endpoint& endpoint, const MpMobUpdateData& message);
	void MpPlayerMobCreatedHandler(const asio::ip::udp::endpoint& endpoint, const MpPlayerMobCreated& message);
	void MpSoundHandler(const asio::ip::udp::endpoint& endpoint, const MpSound& message);
	void MpUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpUpdate& message);

	void tick(float dt);

	std::map<uint64_t, MobPosData>::iterator getMob(uint64_t id);
	void createMob(uint64_t id);

	MpLink link;

	std::map<uint64_t, MobPosData> mobs;

	uint64_t player_mob_id;
};

