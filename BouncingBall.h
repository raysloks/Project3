#pragma once

#include "CustomBehaviour.h"

class BouncingBall :
	public CustomBehaviour
{
public:
	void start();

	void tick(float dt);

	float z;
	Vec2 v;
	float vz;

	Reference<Sprite> visual;
	
	void onCollision(const Collision & collision);

private:
	void update_visual();
};

