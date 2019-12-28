#pragma once

#include "CustomBehaviour.h"

class SpriteRenderSystem;

class Player :
	public CustomBehaviour
{
public:
	void tick(float dt);

	SpriteRenderSystem * srs;
};

