#include "MobPosHandler.h"

#include <iostream>

#include "GameKeyBinding.h"
#include "SpriteAnimator.h"

MobPosHandler::MobPosHandler()
{
	link.handler = this;
	link.Open(asio::ip::udp::endpoint(asio::ip::address_v6::any(), 0));

	link.Receive();

	player_mob_id = -1;

	initialized = false;

	heartbeat_timer = 0.0f;

	endpoint = asio::ip::udp::endpoint(asio::ip::address::from_string("2a01:7e01::f03c:92ff:fe8e:50b4"), 43857);
}

void MobPosHandler::MpActionCommandHandler(const asio::ip::udp::endpoint & endpoint, const MpActionCommand & message)
{
}

void MobPosHandler::MpAttackHandler(const asio::ip::udp::endpoint & endpoint, const MpAttack & message)
{
}

void MobPosHandler::MpAttackCommandHandler(const asio::ip::udp::endpoint & endpoint, const MpAttackCommand & message)
{
}

void MobPosHandler::MpAttackMoveCommandHandler(const asio::ip::udp::endpoint & endpoint, const MpAttackMoveCommand & message)
{
}

void MobPosHandler::MpCancelCommandHandler(const asio::ip::udp::endpoint & endpoint, const MpCancelCommand & message)
{
}

void MobPosHandler::MpChatHandler(const asio::ip::udp::endpoint & endpoint, const MpChat & message)
{
}

void MobPosHandler::MpCommandHandler(const asio::ip::udp::endpoint & endpoint, const MpCommand & message)
{
}

void MobPosHandler::MpDamageHandler(const asio::ip::udp::endpoint & endpoint, const MpDamage & message)
{
	auto mob = mobs.find(message.id);
	if (mob != mobs.end())
	{
		mob->second.mob->onDamaged(message.damage);
	}
}

void MobPosHandler::MpDirectionTargetActionCommandHandler(const asio::ip::udp::endpoint & endpoint, const MpDirectionTargetActionCommand & message)
{
}

void MobPosHandler::MpGuidHandler(const asio::ip::udp::endpoint & endpoint, const MpGuid & message)
{
}

void MobPosHandler::MpLinearResourceChangeHandler(const asio::ip::udp::endpoint & endpoint, const MpLinearResourceChange & message)
{
}

std::vector<std::string> temp = { "bone_boy.png", "imp.png" };

void MobPosHandler::MpMobHealthUpdateHandler(const asio::ip::udp::endpoint & endpoint, const MpMobHealthUpdate & message)
{
	auto mob = getMob(message.id);
	auto&& data = mob->second;

	data.mob->hp.current = message.current;
	data.mob->hp.cap = message.cap;
	data.mob->hp.changes = message.changes;
}

void MobPosHandler::MpMobSpriteUpdateHandler(const asio::ip::udp::endpoint & endpoint, const MpMobSpriteUpdate & message)
{
	/*if (message.id == player_mob_id)
		return;*/

	auto mob = getMob(message.id);
	mob->second.mob->getComponent<Sprite>()->sheet = SpriteSheet::get(temp[message.sprite]);
}

void MobPosHandler::MpMobStateUpdateHandler(const asio::ip::udp::endpoint & endpoint, const MpMobStateUpdate & message)
{
}

void MobPosHandler::MpMobTeamUpdateHandler(const asio::ip::udp::endpoint & endpoint, const MpMobTeamUpdate & message)
{
}

void MobPosHandler::MpMobUpdateHandler(const asio::ip::udp::endpoint & endpoint, const MpMobUpdate & message)
{
}

void MobPosHandler::MpMobUpdateDataHandler(const asio::ip::udp::endpoint & endpoint, const MpMobUpdateData & message)
{
}

void MobPosHandler::MpMoveCommandHandler(const asio::ip::udp::endpoint & endpoint, const MpMoveCommand & message)
{
}

void MobPosHandler::MpPathHandler(const asio::ip::udp::endpoint & endpoint, const MpPath & message)
{
	auto mob = getMob(message.id);
	auto&& data = mob->second;

	data.path.clear();
	data.path.points = message.points;
	if (data.path.points.size() > 0)
	{
		data.path.distances.resize(data.path.points.size() - 1);
		for (size_t i = 0; i < data.path.distances.size(); ++i)
			data.path.distances[i] = (data.path.points[i + 1] - data.path.points[i]).Len();
	}
	int64_t now = std::chrono::steady_clock::now().time_since_epoch().count();
	int64_t diff = message.time - (now + time_offset);
	float speed = 1.5f;
	data.path.move(diff * 0.000000001 * speed);
}

void MobPosHandler::MpPlayerMobCreatedHandler(const asio::ip::udp::endpoint & endpoint, const MpPlayerMobCreated & message)
{
	player_mob_id = message.id;
}

void MobPosHandler::MpPointTargetActionCommandHandler(const asio::ip::udp::endpoint & endpoint, const MpPointTargetActionCommand & message)
{
}

void MobPosHandler::MpSoundHandler(const asio::ip::udp::endpoint & endpoint, const MpSound & message)
{
}

void MobPosHandler::MpStopCommandHandler(const asio::ip::udp::endpoint & endpoint, const MpStopCommand & message)
{
}

void MobPosHandler::MpUnitTargetActionCommandHandler(const asio::ip::udp::endpoint & endpoint, const MpUnitTargetActionCommand & message)
{
}

void MobPosHandler::MpUpdateHandler(const asio::ip::udp::endpoint & endpoint, const MpUpdate & message)
{
	int64_t now = std::chrono::steady_clock::now().time_since_epoch().count();
	time_offset = message.time - now;
}

void MobPosHandler::tick(float dt)
{
	int64_t now = std::chrono::steady_clock::now().time_since_epoch().count();
	time = now + time_offset;

	if (!initialized)
	{
		initialized = true;

		engine->input->addKeyDownCallback(KB_MOVE_ATTACK_CURSOR, [this]()
			{
				int64_t now = std::chrono::steady_clock::now().time_since_epoch().count();

				Vec2 target = engine->srs->screenToWorld(engine->input->getCursor());

				Reference<NetworkMob> target_mob;
				auto in_range = engine->cs->overlapCircle(target + Vec2(4.0f), 8.0f);
				for (auto i : in_range)
				{
					auto mob = i.second->getComponent<NetworkMob>();
					if (mob && mob->id != player_mob_id)
					{
						target_mob = mob;
						break;
					}
				}

				if (target_mob)
				{
					MpAttackCommand message;
					message.command.time = now + time_offset;
					message.target = target_mob->id;
					link.Send(endpoint, message);
				}
				else
				{
					MpMoveCommand message;
					message.command.time = now + time_offset;
					message.target = target / 16.0f + 0.5f;
					link.Send(endpoint, message);

					// create poof
					{
						auto entity = level->add_entity();
						entity->xy = target;

						auto sprite = level->sprites.add("blink.png");
						sprite->sort = 256;
						sprite->sheet->columns = 4;
						Component::attach(sprite, entity);

						auto animator = level->add<SpriteAnimator>(15.0f);
						animator->destroy = true;
						Component::attach(animator, entity);
					}
				}
			});

		engine->input->addKeyDownCallback(KB_STOP_MOVE, [this]()
			{
				int64_t now = std::chrono::steady_clock::now().time_since_epoch().count();

				MpStopCommand message;
				message.command.time = now + time_offset;
				link.Send(endpoint, message);
			});

		engine->input->addKeyDownCallback(KB_CANCEL_ACTION, [this]()
			{
				int64_t now = std::chrono::steady_clock::now().time_since_epoch().count();

				MpCancelCommand message;
				message.command.time = now + time_offset;
				link.Send(endpoint, message);
			});

		for (size_t i = 0; i < 10; ++i)
		{
			engine->input->addKeyDownCallback(KB_ACTION_0 + i, [this, i]()
				{
					int64_t now = std::chrono::steady_clock::now().time_since_epoch().count();

					Vec2 target = engine->srs->screenToWorld(engine->input->getCursor());

					Reference<NetworkMob> target_mob;
					auto in_range = engine->cs->overlapCircle(target + Vec2(4.0f), 8.0f);
					for (auto i : in_range)
					{
						auto mob = i.second->getComponent<NetworkMob>();
						if (mob)
						{
							target_mob = mob;
							break;
						}
					}

					if (target_mob)
					{
						MpUnitTargetActionCommand message;
						message.action.command.time = now + time_offset;
						message.action.action = i;
						message.target = target_mob->id;
						link.Send(endpoint, message);
					}
					else
					{
						MpActionCommand message;
						message.command.time = now + time_offset;
						message.action = i;
						link.Send(endpoint, message);
					}
				});
		}
	}

	for (auto& i : mobs)
	{
		i.second.tick(dt);
	}

	auto player_mob = mobs.find(player_mob_id);
	if (player_mob != mobs.end())
	{
		engine->srs->camera_position = player_mob->second.mob->entity->xy;
	}

	heartbeat_timer -= dt;
	if (heartbeat_timer <= 0.0f)
	{
		MpUpdate message;
		link.Send(endpoint, message);
		heartbeat_timer += 2.5f;
	}

	if (isnan(heartbeat_timer))
		heartbeat_timer = 0.0f;
	if (heartbeat_timer > 4.0f || heartbeat_timer < -4.0f)
		heartbeat_timer = 0.0f;
}

std::map<uint64_t, MobPosData>::iterator MobPosHandler::getMob(uint64_t id)
{
	auto mob = mobs.find(id);
	if (mob != mobs.end())
		return mob;
	createMob(id);
	return mobs.find(id);
}

void MobPosHandler::createMob(uint64_t id)
{
	auto entity = level->add_entity();

	auto sprite = level->sprites.add("uu.png");
	Component::attach(sprite, entity);

	auto mob = level->add<NetworkMob>();
	mob->id = id;
	Component::attach(mob, entity);

	auto collider = level->circle_colliders.add(3.0f);
	collider->layers = 0b10;
	Component::attach(collider, entity);

	auto mob_entity = entity;

	// create shadow
	{
		auto entity = level->add_entity();
		Entity::adopt(entity, mob_entity);

		auto sprite = level->sprites.add("shadow4_iso.png");
		sprite->sort = 0;
		sprite->color = SDL_Color({ 0, 0, 0, 32 });
		Component::attach(sprite, entity);
	}

	MobPosData data;
	data.mob = mob;

	mobs.emplace(std::make_pair(id, data));
}

//const float lower_margin = -5.25f;
//const float nominal_margin_lower = -4.25f;
//const float upper_margin = -2.25f;
//const float nominal_margin_upper = -3.25f;

const float lower_margin = -4.0f;
const float nominal_margin_lower = -4.0f;
const float upper_margin = -2.0f;
const float nominal_margin_upper = -2.0f;

//const float lower_margin = -10.0f;
//const float nominal_margin_lower = -10.0f;
//const float upper_margin = -6.0f;
//const float nominal_margin_upper = -6.0f;

void MobPosData::tick(float dt)
{
	float speed = 1.5f;

	path.move(dt * speed);

	mob->entity->xy = path.getPosition() * 16.0f - 8.0f;

	mob->v = Vec2();
	if (!path.finished())
	{
		mob->move = path.getDirection();
		mob->v = path.getDirection() * speed * 16.0f;
	}
}
