#pragma once

#include "CustomBehaviour.h"

class BouncingBall :
	public CustomBehaviour
{
public:
	void tick(float dt);

	float z;
	Vec2 v;
	float vz;

	Reference<Sprite> visual;

	std::function<void(const Collision&)> on_collision;

private:
	void update_visual();
};

