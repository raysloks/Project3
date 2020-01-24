#pragma once

#include "System.h"

#include "Sprite.h"

class SpriteRenderSystem :
	public System
{
public:
	SpriteRenderSystem(SDL_Renderer * render);

	Vec2 screenToWorld(const Vec2& screen_position) const;
	Vec2 worldToScreen(const Vec2& world_position) const;

	void tick(float dt);

	Vec2 camera_position;

	SDL_Renderer * render;

	int getWidth() const;
	int getHeight() const;

	int getScale() const;

private:
	SDL_Texture * offscreen;

	bool use_offscreen;

	int target_w, target_h;
	int render_w, render_h;
	int screen_w, screen_h;
	int effective_w, effective_h;
	int scale, raw_scale;
};

