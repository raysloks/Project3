#include "ModelAnimator.h"

ModelAnimator::ModelAnimator() : speed(1.0f), time(0.0f), destroy(false)
{
}

ModelAnimator::ModelAnimator(const std::string& action_name, float speed, float time, bool destroy) : action_name(action_name), speed(speed), time(time), destroy(destroy)
{
}

void ModelAnimator::tick(float dt)
{
	time += dt * speed;
	if (auto model = entity->getComponent<ModelRenderer>())
	{
		if (model->animation && model->animation->loaded)
		{
			model->setDirty();
			auto& action = model->animation->actions[action_name];
			if (destroy && time >= action.length)
			{
				level->remove_entity(entity);
				return;
			}
			time = fmodf(time, action.length);
			if (model->pose.bones.empty())
				model->pose = model->animation->base_pose;
			action.getPose(model->pose, model->animation->base_pose, time);
		}
	}
}
