#include "MpLink.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

// Application should implement this class using the prototypes in HandlerPrototypes.h
#include "../MobPosHandler.h"

const uint32_t MpLink::crc = 0xdf6613ef;
MpLink::MpLink() : io_context(), socket(io_context)
{
}

void MpLink::Open(const asio::ip::udp::endpoint& endpoint)
{
	socket.open(endpoint.protocol());
	socket.bind(endpoint);
}

void MpLink::Receive()
{
	std::thread t([this]()
		{
			while (true)
			{
				try
				{
					asio::streambuf buffer(65507); 
					asio::ip::udp::endpoint endpoint;
					buffer.commit(socket.receive_from(buffer.prepare(65507), endpoint));
					Dispatch(buffer, endpoint);
				}
				catch (...)
				{
				}
			}
		}
	);
	t.detach();
}

void MpLink::Connect(const asio::ip::udp::endpoint& endpoint)
{
	std::shared_ptr<asio::streambuf> buffer = std::make_shared<asio::streambuf>();
	std::ostream os(buffer.get());
	os.put(0);
	os.write((const char*)&crc, sizeof(crc));
	os.put(0);
	socket.async_send_to(buffer->data(), endpoint, [buffer](const asio::error_code&, size_t) {});
}

void MpLink::Dispatch(asio::streambuf& buffer, const asio::ip::udp::endpoint& endpoint)
{
	int64_t time = std::chrono::steady_clock::now().time_since_epoch().count();
	std::istream is(&buffer);
	char c = is.get();
	if (c == 0)
		{
			uint32_t remote_crc;
			is.read((char*)&remote_crc, sizeof(remote_crc));
			if (remote_crc != crc)
				return;
			connections[endpoint] = time;
			switch (is.get())
			{
				case 0:
				{
					std::shared_ptr<asio::streambuf> buffer = std::make_shared<asio::streambuf>();
					std::ostream os(buffer.get());
					os.put(0);
					os.write((const char*)&crc, sizeof(crc));
					os.put(1);
					socket.async_send_to(buffer->data(), endpoint, [buffer](const asio::error_code&, size_t) {});
					handler->AcceptHandler(endpoint);
					break;
				}
				case 1:
				{
					handler->ConnectHandler(endpoint);
					break;
				}
				default:
					break;
			}
			return;
		}
	auto it = connections.find(endpoint);
	if (it == connections.end())
		return;
	if (time - it->second > 10'000'000'000)
	{
		connections.erase(it);
		return;
	}
	connections[endpoint] = time;
	switch (c)
	{
	case 5:
	{
		MpAuthentication message;
		message.deserialize(is);
		handler->MpAuthenticationHandler(endpoint, message);
		break;
	}
	case 7:
	{
		MpChat message;
		message.deserialize(is);
		handler->MpChatHandler(endpoint, message);
		break;
	}
	case 9:
	{
		MpDamage message;
		message.deserialize(is);
		handler->MpDamageHandler(endpoint, message);
		break;
	}
	case 11:
	{
		MpMobHealthUpdate message;
		message.deserialize(is);
		handler->MpMobHealthUpdateHandler(endpoint, message);
		break;
	}
	case 12:
	{
		MpMobSpriteUpdate message;
		message.deserialize(is);
		handler->MpMobSpriteUpdateHandler(endpoint, message);
		break;
	}
	case 13:
	{
		MpMobStatsUpdate message;
		message.deserialize(is);
		handler->MpMobStatsUpdateHandler(endpoint, message);
		break;
	}
	case 14:
	{
		MpMobTeamUpdate message;
		message.deserialize(is);
		handler->MpMobTeamUpdateHandler(endpoint, message);
		break;
	}
	case 15:
	{
		MpMobTypeUpdate message;
		message.deserialize(is);
		handler->MpMobTypeUpdateHandler(endpoint, message);
		break;
	}
	case 16:
	{
		MpMobUpdate message;
		message.deserialize(is);
		handler->MpMobUpdateHandler(endpoint, message);
		break;
	}
	case 17:
	{
		MpMobUpdateData message;
		message.deserialize(is);
		handler->MpMobUpdateDataHandler(endpoint, message);
		break;
	}
	case 19:
	{
		MpPlayerExperienceUpdate message;
		message.deserialize(is);
		handler->MpPlayerExperienceUpdateHandler(endpoint, message);
		break;
	}
	case 20:
	{
		MpPlayerInventoryUpdate message;
		message.deserialize(is);
		handler->MpPlayerInventoryUpdateHandler(endpoint, message);
		break;
	}
	case 21:
	{
		MpPlayerMobAbilitiesUpdate message;
		message.deserialize(is);
		handler->MpPlayerMobAbilitiesUpdateHandler(endpoint, message);
		break;
	}
	case 22:
	{
		MpPlayerMobCreated message;
		message.deserialize(is);
		handler->MpPlayerMobCreatedHandler(endpoint, message);
		break;
	}
	case 24:
	{
		MpSound message;
		message.deserialize(is);
		handler->MpSoundHandler(endpoint, message);
		break;
	}
	case 27:
	{
		MpUpdate message;
		message.deserialize(is);
		handler->MpUpdateHandler(endpoint, message);
		break;
	}
	default:
		break;
	}
}

void MpLink::Send(const asio::ip::udp::endpoint& endpoint, const MpActionCommand& message)
{
	std::shared_ptr<asio::streambuf> buffer = std::make_shared<asio::streambuf>();
	std::ostream os(buffer.get());
	os.put(1);
	message.serialize(os);
	socket.async_send_to(buffer->data(), endpoint, [buffer](const asio::error_code&, size_t) {});
}

void MpLink::Send(const asio::ip::udp::endpoint& endpoint, const MpAttack& message)
{
	std::shared_ptr<asio::streambuf> buffer = std::make_shared<asio::streambuf>();
	std::ostream os(buffer.get());
	os.put(2);
	message.serialize(os);
	socket.async_send_to(buffer->data(), endpoint, [buffer](const asio::error_code&, size_t) {});
}

void MpLink::Send(const asio::ip::udp::endpoint& endpoint, const MpAttackCommand& message)
{
	std::shared_ptr<asio::streambuf> buffer = std::make_shared<asio::streambuf>();
	std::ostream os(buffer.get());
	os.put(3);
	message.serialize(os);
	socket.async_send_to(buffer->data(), endpoint, [buffer](const asio::error_code&, size_t) {});
}

void MpLink::Send(const asio::ip::udp::endpoint& endpoint, const MpAttackMoveCommand& message)
{
	std::shared_ptr<asio::streambuf> buffer = std::make_shared<asio::streambuf>();
	std::ostream os(buffer.get());
	os.put(4);
	message.serialize(os);
	socket.async_send_to(buffer->data(), endpoint, [buffer](const asio::error_code&, size_t) {});
}

void MpLink::Send(const asio::ip::udp::endpoint& endpoint, const MpAuthentication& message)
{
	std::shared_ptr<asio::streambuf> buffer = std::make_shared<asio::streambuf>();
	std::ostream os(buffer.get());
	os.put(5);
	message.serialize(os);
	socket.async_send_to(buffer->data(), endpoint, [buffer](const asio::error_code&, size_t) {});
}

void MpLink::Send(const asio::ip::udp::endpoint& endpoint, const MpCancelCommand& message)
{
	std::shared_ptr<asio::streambuf> buffer = std::make_shared<asio::streambuf>();
	std::ostream os(buffer.get());
	os.put(6);
	message.serialize(os);
	socket.async_send_to(buffer->data(), endpoint, [buffer](const asio::error_code&, size_t) {});
}

void MpLink::Send(const asio::ip::udp::endpoint& endpoint, const MpChat& message)
{
	std::shared_ptr<asio::streambuf> buffer = std::make_shared<asio::streambuf>();
	std::ostream os(buffer.get());
	os.put(7);
	message.serialize(os);
	socket.async_send_to(buffer->data(), endpoint, [buffer](const asio::error_code&, size_t) {});
}

void MpLink::Send(const asio::ip::udp::endpoint& endpoint, const MpCommand& message)
{
	std::shared_ptr<asio::streambuf> buffer = std::make_shared<asio::streambuf>();
	std::ostream os(buffer.get());
	os.put(8);
	message.serialize(os);
	socket.async_send_to(buffer->data(), endpoint, [buffer](const asio::error_code&, size_t) {});
}

void MpLink::Send(const asio::ip::udp::endpoint& endpoint, const MpDirectionTargetActionCommand& message)
{
	std::shared_ptr<asio::streambuf> buffer = std::make_shared<asio::streambuf>();
	std::ostream os(buffer.get());
	os.put(10);
	message.serialize(os);
	socket.async_send_to(buffer->data(), endpoint, [buffer](const asio::error_code&, size_t) {});
}

void MpLink::Send(const asio::ip::udp::endpoint& endpoint, const MpMoveCommand& message)
{
	std::shared_ptr<asio::streambuf> buffer = std::make_shared<asio::streambuf>();
	std::ostream os(buffer.get());
	os.put(18);
	message.serialize(os);
	socket.async_send_to(buffer->data(), endpoint, [buffer](const asio::error_code&, size_t) {});
}

void MpLink::Send(const asio::ip::udp::endpoint& endpoint, const MpPointTargetActionCommand& message)
{
	std::shared_ptr<asio::streambuf> buffer = std::make_shared<asio::streambuf>();
	std::ostream os(buffer.get());
	os.put(23);
	message.serialize(os);
	socket.async_send_to(buffer->data(), endpoint, [buffer](const asio::error_code&, size_t) {});
}

void MpLink::Send(const asio::ip::udp::endpoint& endpoint, const MpStopCommand& message)
{
	std::shared_ptr<asio::streambuf> buffer = std::make_shared<asio::streambuf>();
	std::ostream os(buffer.get());
	os.put(25);
	message.serialize(os);
	socket.async_send_to(buffer->data(), endpoint, [buffer](const asio::error_code&, size_t) {});
}

void MpLink::Send(const asio::ip::udp::endpoint& endpoint, const MpUnitTargetActionCommand& message)
{
	std::shared_ptr<asio::streambuf> buffer = std::make_shared<asio::streambuf>();
	std::ostream os(buffer.get());
	os.put(26);
	message.serialize(os);
	socket.async_send_to(buffer->data(), endpoint, [buffer](const asio::error_code&, size_t) {});
}

void MpLink::Send(const asio::ip::udp::endpoint& endpoint, const MpUpdate& message)
{
	std::shared_ptr<asio::streambuf> buffer = std::make_shared<asio::streambuf>();
	std::ostream os(buffer.get());
	os.put(27);
	message.serialize(os);
	socket.async_send_to(buffer->data(), endpoint, [buffer](const asio::error_code&, size_t) {});
}

