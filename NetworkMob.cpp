#include "NetworkMob.h"

#include "MobTemplate.h"
#include "MobPosHandler.h"
#include "ModelRenderSystem.h"

NetworkMob::NetworkMob()
{
	angle = std::numeric_limits<float>::quiet_NaN();
	animation_time_run = 0.0f;
	animation_time_idle = 0.0f;
	animation_blend = 0.0f;
}

void NetworkMob::start()
{
}

void NetworkMob::tick(float dt)
{
	//Mob::tick(dt);

	float target_angle = -atan2f(move.x, move.y);
	if (isnan(angle))
		angle = target_angle;
	float angle_diff = angle - target_angle;
	angle_diff = fmodf(angle_diff + M_PI * 3.0f, M_PI * 2.0f) - M_PI;
	angle = target_angle + angle_diff * expf(-16.0f * dt);

	entity->rotation = Quaternion(angle, Vec3(0.0f, 0.0f, 1.0f));

	entity->z = tm->getZ(entity->xy);

	auto model = getComponent<ModelRenderer>();

	model->setDirty();
	if (model->animation && model->animation->loaded)
	{
		auto& base_pose = model->animation->base_pose;
		Animation::Pose run_pose = base_pose;
		auto& pose = model->pose;
		pose = base_pose;

		{
			auto& action = model->animation->actions["run"];
			animation_time_run += dt * v.Len() * 30.0f;
			animation_time_run = fmodf(animation_time_run, action.length);
			action.getPose(run_pose, base_pose, animation_time_run);
		}

		{
			auto& action = model->animation->actions["idle"];
			animation_time_idle += dt * 10.0f;
			animation_time_idle = fmodf(animation_time_idle, action.length);
			action.getPose(pose, base_pose, animation_time_idle);
		}

		animation_blend += (v != Vec2() ? -5.0f : 10.0f) * dt;
		animation_blend = fmaxf(0.0f, fminf(1.0f, animation_blend));

		for (size_t i = 0; i < pose.bones.size(); ++i)
		{
			pose.bones[i].matrix *= animation_blend;
			pose.bones[i].matrix += run_pose.bones[i].matrix * (1.0f - animation_blend);
		}

		cast_queue.erase(std::remove_if(cast_queue.begin(), cast_queue.end(), [](const Cast& cast)
			{
				const Ability * ability = Ability::get(cast.ability_id);
				return net->time > cast.start + ability->wind_up + ability->wind_down;
			}
		), cast_queue.end());
		if (cast_queue.size())
		{
			auto& cast = cast_queue.front();
			auto& action = model->animation->actions["attack"];
			const Ability * ability = Ability::get(cast.ability_id);
			float animation_time = (float)(net->time - cast.start) / ability->wind_up;
			action.getPose(pose, base_pose, animation_time * action.length);
		}
	}

	if (net->player_mob_id == id)
	{
		srs->camera_position = entity->xy;
		mrs->camera.position = entity->getPosition() + Vec3(-16.0f, -16.0f, 23.0f);
		mrs->camera.rotation = Quaternion(M_PI * 1.75f, Vec3(0.0f, 0.0f, 1.0f)) * Quaternion(-M_PI * 0.75f, Vec3(1.0f, 0.0f, 0.0f));
		mrs->camera.field_of_view = 25.0f;
		mrs->camera.shift = Vec2(0.0f, -0.125f);
	}
}

#include <iostream>

void NetworkMob::setMobTemplate(uint64_t new_mob_template_id)
{
	mob_template_id = new_mob_template_id;
	mob_template = MobTemplate::get(mob_template_id);
	stats.base = mob_template->stats;
	recalculateStats();

	hp.set_cap(stat_cache.hp);
}
