#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <asio.hpp>

#include "MpAttack.h"
#include "MpChat.h"
#include "MpDamage.h"
#include "MpGuid.h"
#include "MpMobSpriteUpdate.h"
#include "MpMobUpdate.h"
#include "MpMobUpdateData.h"
#include "MpPlayerMobCreated.h"
#include "MpSound.h"
#include "MpUpdate.h"

class MobPosHandler;

class MpLink
{
public:
	MobPosHandler * handler;
	MpLink();
	void Open(const asio::ip::udp::endpoint& endpoint);
	void Receive();
	void Dispatch(asio::streambuf& buffer, const asio::ip::udp::endpoint& endpoint);
	void Send(const asio::ip::udp::endpoint& endpoint, const MpAttack& message);
	void Send(const asio::ip::udp::endpoint& endpoint, const MpChat& message);
	void Send(const asio::ip::udp::endpoint& endpoint, const MpDamage& message);
	void Send(const asio::ip::udp::endpoint& endpoint, const MpGuid& message);
	void Send(const asio::ip::udp::endpoint& endpoint, const MpMobSpriteUpdate& message);
	void Send(const asio::ip::udp::endpoint& endpoint, const MpMobUpdate& message);
	void Send(const asio::ip::udp::endpoint& endpoint, const MpMobUpdateData& message);
	void Send(const asio::ip::udp::endpoint& endpoint, const MpPlayerMobCreated& message);
	void Send(const asio::ip::udp::endpoint& endpoint, const MpSound& message);
	void Send(const asio::ip::udp::endpoint& endpoint, const MpUpdate& message);
	static const uint32_t crc;
private:
	asio::io_context io_context;
	asio::ip::udp::socket socket;
};
