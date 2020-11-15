#include "MobPosHandler.h"

#include <iostream>

#include "GameKeyBinding.h"
#include "SpriteAnimator.h"
#include "HealthDisplay.h"

MobPosHandler::MobPosHandler()
{
	link.handler = this;
	link.Open(asio::ip::udp::endpoint(asio::ip::address_v6::any(), 0));

	link.Receive();

	player_mob_id = -1;

	initialized = false;

	heartbeat_timer = 0.0f;

	endpoint = asio::ip::udp::endpoint(asio::ip::address::from_string("2a01:7e01::f03c:92ff:fe8e:50b4"), 43857);
	link.Connect(endpoint);
}

void MobPosHandler::AcceptHandler(const asio::ip::udp::endpoint & endpoint)
{
}

void MobPosHandler::ConnectHandler(const asio::ip::udp::endpoint & endpoint)
{
}

void MobPosHandler::MpChatHandler(const asio::ip::udp::endpoint & endpoint, const MpChat & message)
{
	std::cout << message.message << std::endl;
}

void MobPosHandler::MpDamageHandler(const asio::ip::udp::endpoint & endpoint, const MpDamage & message)
{
	mutex.lock();
	auto mob = mobs.find(message.id);
	if (mob != mobs.end())
	{
		mob->second.mob->onDamaged(message.damage);
	}
	mutex.unlock();
}

std::vector<std::string> temp = { "bone_boy.png", "imp.png" };

void MobPosHandler::MpMobHealthUpdateHandler(const asio::ip::udp::endpoint & endpoint, const MpMobHealthUpdate & message)
{
}

void MobPosHandler::MpMobSpriteUpdateHandler(const asio::ip::udp::endpoint & endpoint, const MpMobSpriteUpdate & message)
{
}

void MobPosHandler::MpMobStateUpdateHandler(const asio::ip::udp::endpoint & endpoint, const MpMobStateUpdate & message)
{
}

void MobPosHandler::MpMobStatsUpdateHandler(const asio::ip::udp::endpoint & endpoint, const MpMobStatsUpdate & message)
{
}

void MobPosHandler::MpMobTeamUpdateHandler(const asio::ip::udp::endpoint & endpoint, const MpMobTeamUpdate & message)
{
}

void MobPosHandler::MpMobUpdateHandler(const asio::ip::udp::endpoint & endpoint, const MpMobUpdate & message)
{
	auto it = getMob(message.id);
	auto&& data = it->second;
	auto&& mob = *data.mob;

	if (message.stats)
	{
		mob.base_stats = *message.stats;
		mob.recalculateStats();
	}

	if (message.hp)
	{
		mob.hp.current = message.hp->current;
		mob.hp.cap = message.hp->cap;
		mob.hp.changes = message.hp->changes;
	}

	if (message.path)
	{
		data.path.clear();
		data.path.points = message.path->points;
		if (data.path.points.size() > 0)
		{
			data.path.distances.resize(data.path.points.size() - 1);
			for (size_t i = 0; i < data.path.distances.size(); ++i)
				data.path.distances[i] = (data.path.points[i + 1] - data.path.points[i]).Len();
		}
		int64_t now = std::chrono::steady_clock::now().time_since_epoch().count();
		int64_t diff = message.path->time - (now + time_offset);
		float speed = mob.stats.move_speed * mob.stats.move_multiplier;
		data.path.move(diff * 0.000000001 * speed);
	}

	if (message.state)
	{
		mob.team = message.state->team;
		mob.getComponent<Sprite>()->sheet = SpriteSheet::get(temp[message.state->sprite]);
	}
}

void MobPosHandler::MpMobUpdateDataHandler(const asio::ip::udp::endpoint & endpoint, const MpMobUpdateData & message)
{
}

void MobPosHandler::MpPlayerMobCreatedHandler(const asio::ip::udp::endpoint & endpoint, const MpPlayerMobCreated & message)
{
	player_mob_id = message.id;
	level->get<HealthDisplay>()->player = getMob(message.id)->second.mob;
}

void MobPosHandler::MpSoundHandler(const asio::ip::udp::endpoint & endpoint, const MpSound & message)
{
}

void MobPosHandler::MpUpdateHandler(const asio::ip::udp::endpoint & endpoint, const MpUpdate & message)
{
	int64_t now = std::chrono::steady_clock::now().time_since_epoch().count();
	int64_t offset = message.time - now;
	int64_t diff = time_offset - offset;
	if (abs(diff) > 5'000'000)
		time_offset = offset;
	link.Send(endpoint, message);
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
	float speed = mob->stats.move_speed * mob->stats.move_multiplier;

	path.move(dt * speed);

	mob->entity->xy = path.getPosition() * 16.0f - 8.0f;

	mob->v = Vec2();
	if (!path.finished())
	{
		mob->move = path.getDirection();
		mob->v = path.getDirection() * speed * 16.0f;
	}
}
