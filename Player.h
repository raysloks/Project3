#pragma once

#include "CustomBehaviour.h"

#include <memory>
#include <functional>

class SpriteRenderSystem;

class Collision;

class Player :
	public CustomBehaviour
{
public:
	void tick(float dt);

	SpriteRenderSystem * srs;

	std::shared_ptr<std::function<void(const Collision&)>> on_collision;
};

