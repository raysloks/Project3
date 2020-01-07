#pragma once

#include <cstdint>
#include <memory>

#include "GroundEffectChunk.h"

#include "Sprite.h"

class SpriteSheet;

class Tile
{
public:
	std::shared_ptr<SpriteSheet> makeEffectSheet();
	void refreshEffectSprite(const Vec2& p);
	
	uint32_t tile;

	GroundEffectChunk effects;

	Reference<Sprite> effect_sprite;
};

