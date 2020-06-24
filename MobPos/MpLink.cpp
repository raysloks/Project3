#include "MpLink.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

// Application should implement this class using the prototypes in HandlerPrototypes.h
#include "../MobPosHandler.h"

const uint32_t MpLink::crc = 0xa9c0f6c1;
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
			asio::streambuf buffer(65507); 
			asio::ip::udp::endpoint endpoint;
			while (true)
			{
				buffer.commit(socket.receive_from(buffer.prepare(65507), endpoint));
				Dispatch(buffer, endpoint);
			}
		}
	);
	t.detach();
}

void MpLink::Dispatch(asio::streambuf& buffer, const asio::ip::udp::endpoint& endpoint)
{
	std::istream is(&buffer);
	switch (is.get())
	{
		case 0:
		{
			uint32_t remote_crc;
			is.read((char*)&remote_crc, sizeof(remote_crc));
			if (remote_crc != crc)
				break;
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
					break;
				}
				case 1:
				{
					break;
				}
				default:
					break;
			}
			break;
		}
	case 1:
	{
		MpAttack message;
		message.deserialize(is);
		handler->MpAttackHandler(endpoint, message);
		break;
	}
	case 2:
	{
		MpChat message;
		message.deserialize(is);
		handler->MpChatHandler(endpoint, message);
		break;
	}
	case 3:
	{
		MpDamage message;
		message.deserialize(is);
		handler->MpDamageHandler(endpoint, message);
		break;
	}
	case 4:
	{
		MpGuid message;
		message.deserialize(is);
		handler->MpGuidHandler(endpoint, message);
		break;
	}
	case 5:
	{
		MpMobSpriteUpdate message;
		message.deserialize(is);
		handler->MpMobSpriteUpdateHandler(endpoint, message);
		break;
	}
	case 6:
	{
		MpMobUpdate message;
		message.deserialize(is);
		handler->MpMobUpdateHandler(endpoint, message);
		break;
	}
	case 7:
	{
		MpMobUpdateData message;
		message.deserialize(is);
		handler->MpMobUpdateDataHandler(endpoint, message);
		break;
	}
	case 8:
	{
		MpPlayerMobCreated message;
		message.deserialize(is);
		handler->MpPlayerMobCreatedHandler(endpoint, message);
		break;
	}
	case 9:
	{
		MpSound message;
		message.deserialize(is);
		handler->MpSoundHandler(endpoint, message);
		break;
	}
	case 10:
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

void MpLink::Send(const asio::ip::udp::endpoint& endpoint, const MpAttack& message)
{
	std::shared_ptr<asio::streambuf> buffer = std::make_shared<asio::streambuf>();
	std::ostream os(buffer.get());
	os.put(1);
	message.serialize(os);
	socket.async_send_to(buffer->data(), endpoint, [buffer](const asio::error_code&, size_t) {});
}

void MpLink::Send(const asio::ip::udp::endpoint& endpoint, const MpChat& message)
{
	std::shared_ptr<asio::streambuf> buffer = std::make_shared<asio::streambuf>();
	std::ostream os(buffer.get());
	os.put(2);
	message.serialize(os);
	socket.async_send_to(buffer->data(), endpoint, [buffer](const asio::error_code&, size_t) {});
}

void MpLink::Send(const asio::ip::udp::endpoint& endpoint, const MpDamage& message)
{
	std::shared_ptr<asio::streambuf> buffer = std::make_shared<asio::streambuf>();
	std::ostream os(buffer.get());
	os.put(3);
	message.serialize(os);
	socket.async_send_to(buffer->data(), endpoint, [buffer](const asio::error_code&, size_t) {});
}

void MpLink::Send(const asio::ip::udp::endpoint& endpoint, const MpGuid& message)
{
	std::shared_ptr<asio::streambuf> buffer = std::make_shared<asio::streambuf>();
	std::ostream os(buffer.get());
	os.put(4);
	message.serialize(os);
	socket.async_send_to(buffer->data(), endpoint, [buffer](const asio::error_code&, size_t) {});
}

void MpLink::Send(const asio::ip::udp::endpoint& endpoint, const MpMobSpriteUpdate& message)
{
	std::shared_ptr<asio::streambuf> buffer = std::make_shared<asio::streambuf>();
	std::ostream os(buffer.get());
	os.put(5);
	message.serialize(os);
	socket.async_send_to(buffer->data(), endpoint, [buffer](const asio::error_code&, size_t) {});
}

void MpLink::Send(const asio::ip::udp::endpoint& endpoint, const MpMobUpdate& message)
{
	std::shared_ptr<asio::streambuf> buffer = std::make_shared<asio::streambuf>();
	std::ostream os(buffer.get());
	os.put(6);
	message.serialize(os);
	socket.async_send_to(buffer->data(), endpoint, [buffer](const asio::error_code&, size_t) {});
}

void MpLink::Send(const asio::ip::udp::endpoint& endpoint, const MpMobUpdateData& message)
{
	std::shared_ptr<asio::streambuf> buffer = std::make_shared<asio::streambuf>();
	std::ostream os(buffer.get());
	os.put(7);
	message.serialize(os);
	socket.async_send_to(buffer->data(), endpoint, [buffer](const asio::error_code&, size_t) {});
}

void MpLink::Send(const asio::ip::udp::endpoint& endpoint, const MpPlayerMobCreated& message)
{
	std::shared_ptr<asio::streambuf> buffer = std::make_shared<asio::streambuf>();
	std::ostream os(buffer.get());
	os.put(8);
	message.serialize(os);
	socket.async_send_to(buffer->data(), endpoint, [buffer](const asio::error_code&, size_t) {});
}

void MpLink::Send(const asio::ip::udp::endpoint& endpoint, const MpSound& message)
{
	std::shared_ptr<asio::streambuf> buffer = std::make_shared<asio::streambuf>();
	std::ostream os(buffer.get());
	os.put(9);
	message.serialize(os);
	socket.async_send_to(buffer->data(), endpoint, [buffer](const asio::error_code&, size_t) {});
}

void MpLink::Send(const asio::ip::udp::endpoint& endpoint, const MpUpdate& message)
{
	std::shared_ptr<asio::streambuf> buffer = std::make_shared<asio::streambuf>();
	std::ostream os(buffer.get());
	os.put(10);
	message.serialize(os);
	socket.async_send_to(buffer->data(), endpoint, [buffer](const asio::error_code&, size_t) {});
}

