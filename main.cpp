#include "Engine.h"

#include "Player.h"
#include "Enemy.h"

#include "FrameRate.h"

int main(int argc, char* args[])
{
	Engine engine;

	// create player
	{
		Entity entity;

		Sprite sprite("potato.png");
		entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

		auto player = std::make_shared<Player>();
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

		Sprite sprite("potato_evil.png");
		entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

		auto enemy = std::make_shared<Enemy>();
		entity.addComponent(&**engine.cbs->behaviours.add(enemy));

		Collider collider;
		collider.r = 32.0f;
		entity.addComponent(engine.cs->colliders.add(std::move(collider)));

		engine.entities.emplace_back(std::move(entity));
	}

	// create some props
	for (size_t i = 0; i < 100; ++i)
	{
		Entity entity;
		entity.p.x = i * 100;
		entity.p.y = 100;

		Sprite sprite("circle34.png");
		entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

		Collider collider;
		collider.r = 16.0f;
		entity.addComponent(engine.cs->colliders.add(std::move(collider)));

		engine.entities.emplace_back(std::move(entity));
	}

	// create fps counter
	{
		Entity entity;

		auto fps = std::make_shared<FrameRate>();
		entity.addComponent(&**engine.cbs->behaviours.add(fps));

		engine.entities.emplace_back(std::move(entity));
	}

	engine.run();

	return 0;
}