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
		entity.x = 50;

		auto sprite = Spritesheet::get("dude_one.png");
		sprite->columns = 4;
		sprite->rows = 2;
		entity.addComponent(engine.srs->sprites.add(Sprite(sprite)));

		auto player = std::make_shared<Player>();
		entity.addComponent(&**engine.cbs->behaviours.add(player));

		Collider collider;
		collider.shape = std::make_unique<Circle>(8.0f);
		entity.addComponent(engine.cs->colliders.add(std::move(collider)));

		engine.entities.emplace_back(std::move(entity));
	}

	// create enemy
	for (size_t i = 0; i < 10; ++i)
	{
		Entity entity;
		entity.x = 50 + 200 * i;
		entity.y = 500;

		Sprite sprite("potato_evil.png");
		entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

		auto enemy = std::make_shared<Enemy>();
		entity.addComponent(&**engine.cbs->behaviours.add(enemy));

		Collider collider;
		collider.shape = std::make_unique<Circle>(16.0f);
		entity.addComponent(engine.cs->colliders.add(std::move(collider)));

		engine.entities.emplace_back(std::move(entity));
	}

	// create some props
	for (size_t i = 0; i < 100; ++i)
	{
		Entity entity;
		entity.x = i % 10 * 100;
		entity.y = i / 10 * 100;

		Sprite sprite("tile.png");
		entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

		Collider collider;
		collider.shape = std::make_unique<Rectangle>(Vec2(16.0f, 16.0f));
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