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

	void MpChatHandler(const asio::ip::udp::endpoint& endpoint, const MpChat& message);
	void MpGuidHandler(const asio::ip::udp::endpoint& endpoint, const MpGuid& message);
	void MpMobUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobUpdate& message);
	void MpMobUpdateDataHandler(const asio::ip::udp::endpoint& endpoint, const MpMobUpdateData& message);
	void MpPlayerMobCreatedHandler(const asio::ip::udp::endpoint& endpoint, const MpPlayerMobCreated& message);
	void MpSoundHandler(const asio::ip::udp::endpoint& endpoint, const MpSound& message);
	void MpUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpUpdate& message);

	void tick(float dt);

	MpLink link;

	std::map<uint64_t, MobPosData> mobs;

	uint64_t player_mob_id;
};

