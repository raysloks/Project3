#pragma once

#include "CustomBehaviour.h"

class BouncingBall :
	public CustomBehaviour
{
public:
	void start();

	void tick(float dt);

	Vec3 v;

	float elasticity;

	Reference<Sprite> shadow;
	
	void onCollision(const Collision & collision);

private:
	void update_shadow();
};

