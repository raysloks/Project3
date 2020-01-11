#pragma once

#include "System.h"

#include "Sprite.h"

class SpriteRenderSystem :
	public System
{
public:
	SpriteRenderSystem(SDL_Renderer * render);

	Vec2 screenToWorld(const Vec2& screen_position);
	Vec2 worldToScreen(const Vec2& world_position);

	void tick(float dt);

	ComponentContainer<Sprite> sprites, ui;

	Vec2 camera_position;

	SDL_Renderer * render;

private:
	SDL_Texture * offscreen;

	bool flicker;

	bool use_offscreen;

	int target_w, target_h;
	int w, h;
	int screen_w, screen_h;
	int scale, raw_scale;
};

