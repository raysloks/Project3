#include "MobPosHandler.h"

#include <iostream>

#include "GameKeyBinding.h"
#include "SpriteAnimator.h"
#include "ModelAnimator.h"
#include "HealthDisplay.h"
#include "ActionBar.h"
#include "NamePlateProvider.h"

#include "MobTemplate.h"

#include "ModelRenderSystem.h"

#include <boost/uuid/string_generator.hpp>

MobPosHandler::MobPosHandler()
{
	link.handler = this;
	link.Open(asio::ip::udp::endpoint(asio::ip::address_v6::any(), 0));

	link.Receive();

	player_mob_id = -1;

	initialized = false;

	heartbeat_timer = 0.0f;

	endpoint = asio::ip::udp::endpoint(asio::ip::address::from_string("2a01:7e01::f03c:92ff:fe8e:50b4"), 43857);
	//endpoint = asio::ip::udp::endpoint(asio::ip::address::from_string("172.105.79.194"), 43857);

	boost::uuids::string_generator gen;
	player_character.character_id = gen("9769f33c-0617-42fc-83b5-4c00a606e529");
}

void MobPosHandler::Connect()
{
	link.Connect(endpoint);
}

void MobPosHandler::AcceptHandler(const asio::ip::udp::endpoint & endpoint)
{
}

void MobPosHandler::ConnectHandler(const asio::ip::udp::endpoint & endpoint)
{
	MpAuthentication message;
	message.character_id = player_character.character_id;
	link.Send(endpoint, message);
}

void MobPosHandler::MpAuthenticationHandler(const asio::ip::udp::endpoint & endpoint, const MpAuthentication & message)
{
}

void MobPosHandler::MpChatHandler(const asio::ip::udp::endpoint & endpoint, const MpChat & message)
{
	std::cout << message.message << std::endl;
}

void MobPosHandler::MpCommandQueueAcknowledgeHandler(const asio::ip::udp::endpoint & endpoint, const MpCommandQueueAcknowledge & message)
{
	command_queue.commands.erase(command_queue.commands.begin(), command_queue.commands.begin() + (int16_t)(message.index - command_queue.index));
	command_queue.index = message.index;
}

void MobPosHandler::MpDamageHandler(const asio::ip::udp::endpoint & endpoint, const MpDamage & message)
{
	//std::cout << "OOF" << std::endl;
	/*mutex.lock();
	auto mob = mobs.find(message.id);
	if (mob != mobs.end())
	{
		mob->second.mob->onDamaged(message.damage);
	}
	mutex.unlock();*/
}

void MobPosHandler::MpMobHealthUpdateHandler(const asio::ip::udp::endpoint & endpoint, const MpMobHealthUpdate & message)
{
}

void MobPosHandler::MpMobSpriteUpdateHandler(const asio::ip::udp::endpoint & endpoint, const MpMobSpriteUpdate & message)
{
}

void MobPosHandler::MpMobStatsUpdateHandler(const asio::ip::udp::endpoint & endpoint, const MpMobStatsUpdate & message)
{
}

void MobPosHandler::MpMobTeamUpdateHandler(const asio::ip::udp::endpoint & endpoint, const MpMobTeamUpdate & message)
{
}

void MobPosHandler::MpMobTypeUpdateHandler(const asio::ip::udp::endpoint & endpoint, const MpMobTypeUpdate & message)
{
}

void MobPosHandler::MpMobUpdateHandler(const asio::ip::udp::endpoint & endpoint, const MpMobUpdate & message)
{
	auto it = getMob(message.id);
	auto&& data = it->second;
	auto&& mob = *data.mob;

	if (message.type)
	{
		mob.setMobTemplate(message.type->template_id);
	}

	if (message.auras)
	{
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
		data.path.distances = message.path->distances;
		int64_t diff = time - message.path->time;
		//float speed = mob.getMovementSpeed();
		data.path.move(diff / 1'000'000'000.0);
	}

	if (message.state)
	{
		mob.team = message.state->team;
		//mob.getComponent<Sprite>()->sheet = SpriteSheet::get(temp[message.state->sprite]);
	}

	if (message.cast)
	{
		mob.cast_queue = message.cast->queue;
	}
}

void MobPosHandler::MpMobUpdateDataHandler(const asio::ip::udp::endpoint & endpoint, const MpMobUpdateData & message)
{
}

void MobPosHandler::MpPlayerExperienceUpdateHandler(const asio::ip::udp::endpoint & endpoint, const MpPlayerExperienceUpdate & message)
{
}

void MobPosHandler::MpPlayerInventoryUpdateHandler(const asio::ip::udp::endpoint & endpoint, const MpPlayerInventoryUpdate & message)
{
}

void MobPosHandler::MpPlayerMobAbilitiesUpdateHandler(const asio::ip::udp::endpoint & endpoint, const MpPlayerMobAbilitiesUpdate & message)
{
	auto mob = getMob(message.id)->second.mob.get();
	mob->abilities.resize(message.abilities.size());
	for (size_t i = 0; i < message.abilities.size(); ++i)
	{
		mob->abilities[i].ability = Ability::get(message.abilities[i]);
	}
}

void MobPosHandler::MpPlayerMobCreatedHandler(const asio::ip::udp::endpoint & endpoint, const MpPlayerMobCreated & message)
{
	player_mob_id = message.id;
	level->get<HealthDisplay>()->player = getMob(message.id)->second.mob;
	level->get<ActionBar>()->player = getMob(message.id)->second.mob;
	player_character.name = message.name;
	player_character.equipment = message.inventory.equipment;
	player_character.inventory = message.inventory.inventory;
}

void MobPosHandler::MpSoundHandler(const asio::ip::udp::endpoint & endpoint, const MpSound & message)
{
}

void MobPosHandler::MpUpdateHandler(const asio::ip::udp::endpoint & endpoint, const MpUpdate & message)
{
	int64_t now = std::chrono::steady_clock::now().time_since_epoch().count();
	int64_t offset = message.time - now;
	int64_t diff = time_offset - offset;
	if (abs(diff) > 10'000'000)
	{
		time_offset = offset;
		std::cout << "SYNC" << std::endl;
	}
	link.Send(endpoint, message);
}

void MobPosHandler::tick(float dt)
{
	int64_t now = std::chrono::steady_clock::now().time_since_epoch().count();
	int64_t diff = now + time_offset - time;
	time = now + time_offset;

	if (!initialized)
	{
		initialized = true;

		{
			std::string navmesh_fname = "data/airtown.navmesh.mdl";
			navmesh.Initialize(navmesh_fname);

			{
				auto entity = level->add_entity();
				auto model = level->models.add("airtown.sightmesh.mdl", "testgrid2.png");
				Component::attach(model, entity);
			}

			{
				auto entity = level->add_entity();
				auto model = level->models.add("airtown.backfacetest.mdl", "pixel.png");
				model->uniform_buffer_object.color = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
				Component::attach(model, entity);
			}
		}

		{
			auto normal_cursor = SpriteSheet::get("cursor.png");
			normal_cursor->offset_x = 1;
			normal_cursor->offset_y = 1;
			auto attack_cursor = normal_cursor->makeOutline({ 255, 0, 0, 255 }, { 255, 255, 255, 0 }, 4, 4);
			attack_cursor->offset_x = 5;
			attack_cursor->offset_y = 5;
			engine->mrs->ui->setUpdateCursorCallback([this, normal_cursor, attack_cursor]()
				{
					if (player_mob_id == -1)
						return;

					int64_t now = std::chrono::steady_clock::now().time_since_epoch().count();

					Vec3 target = engine->mrs->screenToWorld(engine->input->getCursor(), mobs[player_mob_id].mob->entity->z);

					Reference<NetworkMob> target_mob;
					auto in_range = engine->cs->overlapCircle(target, 0.5f);
					for (auto& i : in_range)
					{
						auto mob = i.second->getComponent<NetworkMob>();
						if (mob && mob->id != player_mob_id)
						{
							target_mob = mob;
							break;
						}
					}

					if (target_mob)
						engine->setCursor(attack_cursor);
					else
						engine->setCursor(normal_cursor);
				});
		}

		engine->input->addKeyDownCallback(KB_MOVE_ATTACK_CURSOR, [this]()
			{
				if (player_mob_id == -1)
					return;

				int64_t now = std::chrono::steady_clock::now().time_since_epoch().count();

				Vec3 target = engine->mrs->screenToWorld(engine->input->getCursor(), mobs[player_mob_id].mob->entity->z);
				target = navmesh.GetFacePosition(target).position;
				last_path_target = target;

				Reference<NetworkMob> target_mob;
				auto in_range = engine->cs->overlapCircle(target, 0.5f);
				for (auto& i : in_range)
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
					moving = false;

					MpAttackCommand message;
					message.time = time;
					message.target = target_mob->id;
					Queue(message);
				}
				else
				{
					moving = true;

					auto&& mob = mobs[player_mob_id];
					auto&& path = mob.path;

					MpMoveCommand message;
					message.time = time;
					message.position = mob.mob->entity->xyz;
					message.target = target;
					Queue(message);

					last_sent_path_time = time; // change to 'now' when mini fast-forward is working

					//float speed = mob.mob->stats.movement_speed * mob.mob->stats.movement_speed_multiplier;
					//path.move((now + time_offset - time) * 0.000000001 * speed);
					/*if (path.points.size() > 0)
						mob.mob->entity->xy = path.getPosition() * 16.0f - 8.0f;*/
					std::vector<Vec3> path3d = navmesh.GeneratePath(mob.mob->entity->xyz, target);
					path.clear();
					for (auto& point : path3d)
						path.points.push_back(point);
					//path.points = navmesh.findPath(mob.mob->entity->xy, message.target);
					//path.points = { message.target, mob.mob->entity->xy / 16.0f + 0.5f };
					std::reverse(path.points.begin(), path.points.end());
					if (path.points.size() > 0)
					{
						float speed = mob.mob->getMovementSpeed();
						path.distances.resize(path.points.size() - 1);
						for (size_t i = 0; i < path.distances.size(); ++i)
							path.distances[i] = Vec2(path.points[i + 1] - path.points[i]).Len() / speed;
					}
					else
					{
						path.points.push_back(mob.mob->entity->xyz);
					}

					last_sent_path = path;

					// create poof
					{
						auto entity = level->add_entity();
						entity->xyz = path.points.back();

						size_t size = path.points.size();
						if (size >= 2)
						{
							auto diff = path.points[size - 1] - path.points[size - 2];
							entity->rotation = Quaternion(atan2f(diff.y, diff.x), Vec3(0.0f, 0.0f, 1.0f));
						}

						auto model = level->models.add("movement_indicator.mdl", "pixel.png", "movement_indicator.anm");
						model->uniform_buffer_object.color = Vec4(0.0f, 0.7f, 0.0f, 1.0f);
						Component::attach(model, entity);

						auto animator = level->add<ModelAnimator>("move", 112.5f, 0.0f, true);
						Component::attach(animator, entity);
						animator->tick(0.0f);
					}
				}
			});

		engine->input->addKeyDownCallback(KB_STOP_MOVE, [this]()
			{
				int64_t now = std::chrono::steady_clock::now().time_since_epoch().count();

				auto&& mob = mobs[player_mob_id];
				auto&& path = mob.path;

				MpStopCommand message;
				message.time = time;
				message.position = mob.mob->entity->xyz;
				Queue(message);

				path.clear();
				path.points.push_back(mob.mob->entity->xyz);
		});

		engine->input->addKeyDownCallback(KB_CANCEL_ACTION, [this]()
			{
				int64_t now = std::chrono::steady_clock::now().time_since_epoch().count();

				MpCancelCommand message;
				message.time = time;
				Queue(message);
			});

		for (size_t i = 0; i < 10; ++i)
		{
			engine->input->addKeyDownCallback(KB_ACTION_0 + i, [this, i]()
				{
					int64_t now = std::chrono::steady_clock::now().time_since_epoch().count();

					Vec3 target = engine->mrs->screenToWorld(engine->input->getCursor());
					target = navmesh.GetFacePosition(target).position;

					Reference<NetworkMob> target_mob;
					auto in_range = engine->cs->overlapCircle(target, 0.5f);
					for (auto& i : in_range)
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
						message.time = time;
						message.action = i;
						message.target = target_mob->id;
						Queue(message);
					}
					else
					{
						MpActionCommand message;
						message.time = time;
						message.action = i;
						Queue(message);
					}

					// create poof
					{
						auto entity = level->add_entity();
						if (target_mob)
							Entity::adopt(entity, target_mob->entity);
						else
							entity->xyz = target;

						auto model = level->models.add("movement_indicator.mdl", "pixel.png", "movement_indicator.anm");
						model->uniform_buffer_object.color = Vec4(0.7f, 0.0f, 0.0f, 1.0f);
						Component::attach(model, entity);

						auto animator = level->add<ModelAnimator>("move", 112.5f, 0.0f, true);
						Component::attach(animator, entity);
						animator->tick(0.0f);
					}
				});
		}
	}

	if (player_mob_id != -1)
	{
		if (moving && (engine->input->isKeyDown(KB_MOVE_ATTACK_CURSOR) || engine->input->isKeyReleased(KB_MOVE_ATTACK_CURSOR)) && !engine->input->isKeyDown(KB_STOP_MOVE))
		{
			Vec3 target = engine->mrs->screenToWorld(engine->input->getCursor(), last_path_target.z);
			target = navmesh.GetFacePosition(target).position;
			last_path_target = target;

			auto&& mob = mobs[player_mob_id];
			auto&& path = mob.path;

			std::vector<Vec3> path3d = navmesh.GeneratePath(mob.mob->entity->xyz, target);
			path.clear();
			for (auto& point : path3d)
				path.points.push_back(point);
			std::reverse(path.points.begin(), path.points.end());
			if (path.points.size() > 0)
			{
				float speed = mob.mob->getMovementSpeed();
				path.distances.resize(path.points.size() - 1);
				for (size_t i = 0; i < path.distances.size(); ++i)
					path.distances[i] = Vec2(path.points[i + 1] - path.points[i]).Len() / speed;
			}
			else
			{
				path.points.push_back(mob.mob->entity->xyz);
			}

			float dot = Vec2::Dot(path.getFlatDirection(), last_sent_path.getFlatDirection());
			if (dot < 0.0f || time >= last_sent_path_time + 30'000'000 || engine->input->isKeyReleased(KB_MOVE_ATTACK_CURSOR))
			{
				MpMoveCommand message;
				message.time = time;
				message.position = mob.mob->entity->xyz;
				message.target = target;
				Queue(message);

				last_sent_path_time = time;
				last_sent_path = path;
			}
		}
	}

	for (auto& i : mobs)
	{
		i.second.tick(diff * 0.000000001);
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

	auto model = level->models.add("hoodlum.mdl", "hoodlum.png", "hoodlum.anm");
	Component::attach(model, entity);

	auto mob = level->add<NetworkMob>();
	mob->id = id;
	Component::attach(mob, entity);

	auto collider = level->circle_colliders.add(0.1875f);
	collider->layers = 0b10;
	Component::attach(collider, entity);

	auto nameplate = level->add<NamePlateProvider>();
	Component::attach(nameplate, entity);

	MobPosData data;
	data.mob = mob;

	mobs.insert(std::make_pair(id, data));
}

void MobPosData::tick(float dt)
{
	float speed = mob->getMovementSpeed();

	path.move(dt);

	mob->entity->xyz = path.getPosition();

	mob->v = Vec2();
	if (!path.finished())
	{
		mob->move = path.getFlatDirection();
		mob->v = path.getFlatDirection() * speed; // maybe change based on path.distances ?
	}
}
