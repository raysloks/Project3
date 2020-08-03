#pragma once

#include "System.h"

#include "MobPos/MpLink.h"

#include "Engine.h"

#include "NetworkMob.h"

#include "MobPath.h"

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

	void MpActionCommandHandler(const asio::ip::udp::endpoint& endpoint, const MpActionCommand& message);
	void MpAttackHandler(const asio::ip::udp::endpoint& endpoint, const MpAttack& message);
	void MpAttackCommandHandler(const asio::ip::udp::endpoint& endpoint, const MpAttackCommand& message);
	void MpAttackMoveCommandHandler(const asio::ip::udp::endpoint& endpoint, const MpAttackMoveCommand& message);
	void MpCancelCommandHandler(const asio::ip::udp::endpoint& endpoint, const MpCancelCommand& message);
	void MpChatHandler(const asio::ip::udp::endpoint& endpoint, const MpChat& message);
	void MpCommandHandler(const asio::ip::udp::endpoint& endpoint, const MpCommand& message);
	void MpDamageHandler(const asio::ip::udp::endpoint& endpoint, const MpDamage& message);
	void MpGuidHandler(const asio::ip::udp::endpoint& endpoint, const MpGuid& message);
	void MpMobSpriteUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobSpriteUpdate& message);
	void MpMobStateUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobStateUpdate& message);
	void MpMobTeamUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobTeamUpdate& message);
	void MpMobUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobUpdate& message);
	void MpMobUpdateDataHandler(const asio::ip::udp::endpoint& endpoint, const MpMobUpdateData& message);
	void MpMoveCommandHandler(const asio::ip::udp::endpoint& endpoint, const MpMoveCommand& message);
	void MpPathHandler(const asio::ip::udp::endpoint& endpoint, const MpPath& message);
	void MpPlayerMobCreatedHandler(const asio::ip::udp::endpoint& endpoint, const MpPlayerMobCreated& message);
	void MpPointTargetActionCommandHandler(const asio::ip::udp::endpoint& endpoint, const MpPointTargetActionCommand& message);
	void MpSoundHandler(const asio::ip::udp::endpoint& endpoint, const MpSound& message);
	void MpStopCommandHandler(const asio::ip::udp::endpoint& endpoint, const MpStopCommand& message);
	void MpUnitTargetActionCommandHandler(const asio::ip::udp::endpoint& endpoint, const MpUnitTargetActionCommand& message);
	void MpUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpUpdate& message);

	void tick(float dt);

	std::map<uint64_t, MobPosData>::iterator getMob(uint64_t id);
	void createMob(uint64_t id);

	MpLink link;

	std::map<uint64_t, MobPosData> mobs;

	uint64_t player_mob_id;

	int64_t time_offset;

private:

	asio::ip::udp::endpoint endpoint;

	bool initialized;

	float heartbeat_timer;
};

