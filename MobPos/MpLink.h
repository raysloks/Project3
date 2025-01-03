#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <asio.hpp>

#include <map>

#include "MpActionCommand.h"
#include "MpAttack.h"
#include "MpAttackCommand.h"
#include "MpAttackMoveCommand.h"
#include "MpAuraUpdate.h"
#include "MpAuthentication.h"
#include "MpCancelCommand.h"
#include "MpChat.h"
#include "MpCommand.h"
#include "MpCommandQueue.h"
#include "MpCommandQueueAcknowledge.h"
#include "MpCommandWrapper.h"
#include "MpDamage.h"
#include "MpDirectionTargetActionCommand.h"
#include "MpGuid.h"
#include "MpLinearResourceChange.h"
#include "MpMobAurasUpdate.h"
#include "MpMobCastUpdate.h"
#include "MpMobHealthUpdate.h"
#include "MpMobSpriteUpdate.h"
#include "MpMobStateUpdate.h"
#include "MpMobStatsUpdate.h"
#include "MpMobTeamUpdate.h"
#include "MpMobTypeUpdate.h"
#include "MpMobUpdate.h"
#include "MpMobUpdateData.h"
#include "MpMoveCommand.h"
#include "MpPath.h"
#include "MpPlayerExperienceUpdate.h"
#include "MpPlayerInventoryUpdate.h"
#include "MpPlayerMobAbilitiesUpdate.h"
#include "MpPlayerMobCreated.h"
#include "MpPointTargetActionCommand.h"
#include "MpSound.h"
#include "MpStopCommand.h"
#include "MpUnitTargetActionCommand.h"
#include "MpUpdate.h"

class MobPosHandler;

class MpLink
{
public:
	MobPosHandler * handler;
	MpLink();
	void Open(const asio::ip::udp::endpoint& endpoint);
	void Receive();
	void Connect(const asio::ip::udp::endpoint& endpoint);
	void Dispatch(asio::streambuf& buffer, const asio::ip::udp::endpoint& endpoint);
	void Send(const asio::ip::udp::endpoint& endpoint, const MpActionCommand& message);
	void Send(const asio::ip::udp::endpoint& endpoint, const MpAttack& message);
	void Send(const asio::ip::udp::endpoint& endpoint, const MpAttackCommand& message);
	void Send(const asio::ip::udp::endpoint& endpoint, const MpAttackMoveCommand& message);
	void Send(const asio::ip::udp::endpoint& endpoint, const MpAuthentication& message);
	void Send(const asio::ip::udp::endpoint& endpoint, const MpCancelCommand& message);
	void Send(const asio::ip::udp::endpoint& endpoint, const MpChat& message);
	void Send(const asio::ip::udp::endpoint& endpoint, const MpCommand& message);
	void Send(const asio::ip::udp::endpoint& endpoint, const MpCommandQueue& message);
	void Send(const asio::ip::udp::endpoint& endpoint, const MpCommandWrapper& message);
	void Send(const asio::ip::udp::endpoint& endpoint, const MpDirectionTargetActionCommand& message);
	void Send(const asio::ip::udp::endpoint& endpoint, const MpMoveCommand& message);
	void Send(const asio::ip::udp::endpoint& endpoint, const MpPointTargetActionCommand& message);
	void Send(const asio::ip::udp::endpoint& endpoint, const MpStopCommand& message);
	void Send(const asio::ip::udp::endpoint& endpoint, const MpUnitTargetActionCommand& message);
	void Send(const asio::ip::udp::endpoint& endpoint, const MpUpdate& message);
	static const uint32_t crc;
private:
	asio::io_context io_context;
	asio::ip::udp::socket socket;
	std::map<asio::ip::udp::endpoint, int64_t> connections;
};
