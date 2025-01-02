#pragma once

#include "System.h"

#include "MobPos/MpLink.h"

#include "Engine.h"

#include "NetworkMob.h"

#include "MobPath.h"

#include "NavMesh.h"

#include "Character.h"

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

	void Connect();

	void AcceptHandler(const asio::ip::udp::endpoint& endpoint);
	void ConnectHandler(const asio::ip::udp::endpoint& endpoint);
	void MpAuthenticationHandler(const asio::ip::udp::endpoint& endpoint, const MpAuthentication& message);
	void MpChatHandler(const asio::ip::udp::endpoint& endpoint, const MpChat& message);
	void MpCommandQueueAcknowledgeHandler(const asio::ip::udp::endpoint& endpoint, const MpCommandQueueAcknowledge& message);
	void MpDamageHandler(const asio::ip::udp::endpoint& endpoint, const MpDamage& message);
	void MpMobHealthUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobHealthUpdate& message);
	void MpMobSpriteUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobSpriteUpdate& message);
	void MpMobStatsUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobStatsUpdate& message);
	void MpMobTeamUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobTeamUpdate& message);
	void MpMobTypeUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobTypeUpdate& message);
	void MpMobUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobUpdate& message);
	void MpMobUpdateDataHandler(const asio::ip::udp::endpoint& endpoint, const MpMobUpdateData& message);
	void MpPlayerExperienceUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpPlayerExperienceUpdate& message);
	void MpPlayerInventoryUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpPlayerInventoryUpdate& message);
	void MpPlayerMobAbilitiesUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpPlayerMobAbilitiesUpdate& message);
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

	template <class T>
	void Queue(const T& command)
	{
		command_queue.commands.emplace_back(MpCommandWrapper());
		command_queue.commands.back().command = std::make_unique<T>(command);
		Send(command_queue);
	}

	std::mutex mutex;

	NavMesh navmesh;

	Character player_character;

private:

	asio::ip::udp::endpoint endpoint;

	bool initialized;

	float heartbeat_timer;

	MpCommandQueue command_queue;
	
	int64_t last_sent_path_time;
	MobPath last_sent_path;

	Vec3 last_path_target;
	bool moving;
};
