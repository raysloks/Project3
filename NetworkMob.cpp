#include "NetworkMob.h"

#include "MobTemplate.h"
#include "MobPosHandler.h"
#include "ModelRenderSystem.h"

NetworkMob::NetworkMob()
{
	angle = std::numeric_limits<float>::quiet_NaN();
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

	getComponent<ModelRenderer>()->setDirty();

	if (net->player_mob_id == id)
	{
		srs->camera_position = entity->xy;
		mrs->camera_position = entity->getPosition() + Vec3(-16.0f, -16.0f, 23.0f);
		mrs->camera_rotation = Quaternion(M_PI * 1.75f, Vec3(0.0f, 0.0f, 1.0f)) * Quaternion(-M_PI * 0.75f, Vec3(1.0f, 0.0f, 0.0f));
		mrs->field_of_view = 30.0f;
	}
}

#include <iostream>

void NetworkMob::setMobTemplate(uint64_t new_mob_template_id)
{
	mob_template_id = new_mob_template_id;
	mob_template = MobTemplate::get(mob_template_id);
	memcpy(&base_stats, &mob_template->stats, sizeof(MobStatBlock));
	recalculateStats();

	hp.set_cap(stats.hp);
}
