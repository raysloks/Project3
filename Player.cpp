#include "Player.h"

#include "InputSystem.h"

#include <SDL.h>

#include "SpriteRenderSystem.h"

void Player::tick(float dt)
{
	float speed = 300.0f;

	float dx = 0.0f;
	float dy = 0.0f;

	if (InputSystem::input->isKeyDown(SDLK_w))
		dy -= 1.0f;
	if (InputSystem::input->isKeyDown(SDLK_a))
		dx -= 1.0f;
	if (InputSystem::input->isKeyDown(SDLK_s))
		dy += 1.0f;
	if (InputSystem::input->isKeyDown(SDLK_d))
		dx += 1.0f;

	float l = sqrtf(dx * dx + dy * dy);
	if (l != 0.0f)
	{
		dx /= l;
		dy /= l;

		dx *= speed * dt;
		dy *= speed * dt;

		entity->p.x += dx;
		entity->p.y += dy;
	}

	if (srs)
	{
		int w, h;
		SDL_GetRendererOutputSize(srs->render, &w, &h);
		srs->camera_position.x = entity->p.x - w / 2;
		srs->camera_position.y = entity->p.y - h / 2;
	}
}
