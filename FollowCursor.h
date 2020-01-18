#pragma once

#include "CustomBehaviour.h"

class Enemy;

class FollowCursor :
	public CustomBehaviour
{
public:
	void tick(float dt);

	std::shared_ptr<SpriteSheet> current_enemy_sheet;

	Reference<Enemy> current_hover;

	float t;
};

