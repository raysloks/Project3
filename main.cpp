#include "Engine.h"

#include <SDL_image.h>

#include "Player.h"
#include "Enemy.h"

int main(int argc, char* args[])
{
	Engine engine;

	// create player
	{
		Entity entity;

		Sprite sprite;
		sprite.texture = SDL_CreateTextureFromSurface(engine.render, IMG_Load("potato.png"));
		sprite.rect.x = 0;
		sprite.rect.y = 0;
		sprite.rect.w = 128;
		sprite.rect.h = 128;
		entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

		auto player = std::make_shared<Player>();
		player->srs = engine.srs;
		entity.addComponent(&**engine.cbs->behaviours.add(player));

		Collider collider;
		collider.r = 32.0f;
		entity.addComponent(engine.cs->colliders.add(std::move(collider)));

		engine.entities.emplace_back(std::move(entity));
	}

	// create enemy
	{
		Entity entity;
		entity.p.y = 500.0f;

		Sprite sprite;
		sprite.texture = SDL_CreateTextureFromSurface(engine.render, IMG_Load("potato_evil.png"));
		sprite.rect.x = 0;
		sprite.rect.y = 0;
		sprite.rect.w = 128;
		sprite.rect.h = 128;
		entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

		auto enemy = std::make_shared<Enemy>();
		enemy->cs = engine.cs;
		entity.addComponent(&**engine.cbs->behaviours.add(enemy));

		Collider collider;
		collider.r = 32.0f;
		entity.addComponent(engine.cs->colliders.add(std::move(collider)));

		engine.entities.emplace_back(std::move(entity));
	}

	auto texture = SDL_CreateTextureFromSurface(engine.render, SDL_LoadBMP("test.bmp"));

	// create some props
	for (size_t i = 0; i < 100; ++i)
	{
		Entity entity;
		entity.p.x = i * 100;
		entity.p.y = 0;

		Sprite sprite;
		sprite.texture = texture;
		sprite.rect.x = 0;
		sprite.rect.y = 0;
		sprite.rect.w = 32;
		sprite.rect.h = 32;
		entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

		Collider collider;
		collider.r = 16.0f;
		entity.addComponent(engine.cs->colliders.add(std::move(collider)));

		engine.entities.emplace_back(std::move(entity));
	}

	engine.run();

	return 0;
}