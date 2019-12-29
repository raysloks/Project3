#include "Player.h"

#include "InputSystem.h"

#include <SDL.h>

#include "SpriteRenderSystem.h"

#include "Collider.h"

void Player::tick(float dt)
{
	if (!on_collision)
	{
		on_collision = std::make_shared<std::function<void(const Collision&)>>([this](const Collision& collision)
			{
				entity->p -= collision.n * collision.pen;
			});
		entity->getComponent<Collider>()->callbacks.push_back(on_collision);
	}

	float speed = 240.0f;

	Vec2 move;

	if (InputSystem::input->isKeyDown(SDLK_w))
		move.y -= 1.0f;
	if (InputSystem::input->isKeyDown(SDLK_a))
		move.x -= 1.0f;
	if (InputSystem::input->isKeyDown(SDLK_s))
		move.y += 1.0f;
	if (InputSystem::input->isKeyDown(SDLK_d))
		move.x += 1.0f;

	float l = move.Len();
	if (l != 0.0f)
	{
		move /= l;

		move *= speed * dt;

		entity->p += move;
	}

	if (srs)
	{
		srs->camera_position.x = entity->p.x;
		srs->camera_position.y = entity->p.y;
	}
}
