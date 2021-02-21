#pragma once

#include "System.h"

#include "MobPos/MpLink.h"

#include "Engine.h"

#include "NetworkMob.h"

#include "MobPath.h"

#include "Grid.h"

class MobPosData
{
public:
	Reference<NetworkMob> mob;
	MobPath path;

	void tick(float dt);
};

class MobPosHandler :
	public System
{
public:
	MobPosHandler();

	void AcceptHandler(const asio::ip::udp::endpoint& endpoint);
	void ConnectHandler(const asio::ip::udp::endpoint& endpoint);
	void MpChatHandler(const asio::ip::udp::endpoint& endpoint, const MpChat& message);
	void MpDamageHandler(const asio::ip::udp::endpoint& endpoint, const MpDamage& message);
	void MpMobHealthUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobHealthUpdate& message);
	void MpMobSpriteUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobSpriteUpdate& message);
	void MpMobStateUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobStateUpdate& message);
	void MpMobStatsUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobStatsUpdate& message);
	void MpMobTeamUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobTeamUpdate& message);
	void MpMobTypeUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobTypeUpdate& message);
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

	int64_t time, time_offset;

	template <class T>
	void Send(const T& message)
	{
		link.Send(endpoint, message);
	}

	std::mutex mutex;

	Grid grid;

private:

	asio::ip::udp::endpoint endpoint;

	bool initialized;

	float heartbeat_timer;
};
