#include "Player.h"

#include "InputSystem.h"

#include <SDL.h>

void Player::tick(float dt)
{
	float speed = 240.0f;

	if (InputSystem::input->isKeyDown(SDLK_w))
		entity->p.y -= speed * dt;
	if (InputSystem::input->isKeyDown(SDLK_a))
		entity->p.x -= speed * dt;
	if (InputSystem::input->isKeyDown(SDLK_s))
		entity->p.y += speed * dt;
	if (InputSystem::input->isKeyDown(SDLK_d))
		entity->p.x += speed * dt;
}
