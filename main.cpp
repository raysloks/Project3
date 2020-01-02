#include "Engine.h"

#include "Player.h"
#include "Enemy.h"

#include "FrameRate.h"

#include "Tilemap.h"
#include "TilemapShape.h"

#include "GameKeyBinding.h"

#include "Text.h"

int main(int argc, char* args[])
{
	Engine engine;

	engine.input->setKeyBinding(KB_ATTACK, SDLK_SPACE);
	engine.input->setKeyBinding(KB_UP, SDLK_w);
	engine.input->setKeyBinding(KB_LEFT, SDLK_a);
	engine.input->setKeyBinding(KB_DOWN, SDLK_s);
	engine.input->setKeyBinding(KB_RIGHT, SDLK_d);

	// create player
	{
		Entity entity;
		entity.x = 50;
		entity.y = -50;

		auto sprite = Spritesheet::get("dude_one.png");
		sprite->columns = 4;
		sprite->rows = 2;
		entity.addComponent(engine.srs->sprites.add(Sprite(sprite)));

		auto player = std::make_shared<Player>();
		entity.addComponent(&**engine.cbs->behaviours.add(player));

		Collider collider;
		collider.shape = std::make_unique<Circle>(6.0f);
		entity.addComponent(engine.cs->colliders.add(std::move(collider)));

		engine.add_entity(std::move(entity));
	}

	// create enemy
	for (size_t i = 0; i < 10; ++i)
	{
		Entity entity;
		entity.x = 50 + 200 * i;
		entity.y = -500;

		Sprite sprite("potato_evil.png");
		entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

		auto enemy = std::make_shared<Enemy>();
		entity.addComponent(&**engine.cbs->behaviours.add(enemy));

		Collider collider;
		collider.shape = std::make_unique<Circle>(16.0f);
		entity.addComponent(engine.cs->colliders.add(std::move(collider)));

		engine.add_entity(std::move(entity));
	}

	const int w = 100;
	const int h = 100;

	Tilemap tilemap(w, h);
	tilemap.tile_size = Vec2(16.0f, 16.0f);

	for (int x = 0; x < w; ++x)
	{
		for (int y = 0; y < h; ++y)
		{
			tilemap[x][y] = 1;
		}
	}

	for (size_t i = 0; i < 4; ++i)
	{
		int x = 0;
		int y = 0;
		for (size_t i = 0; i < 2000; ++i)
		{
			if (x < 0)
				x = 0;
			if (y < 0)
				y = 0;
			if (x > w - 1)
				x = w - 1;
			if (y > h - 1)
				y = h - 1;
			tilemap[x][y] = 0;
			switch (rand() % 6)
			{
			case 0:
				y -= 1;
				break;
			case 1:
				x -= 1;
				break;
			case 2:
				y += 1;
				break;
			case 3:
				x += 1;
				break;
				break;
			case 4:
				y += 1;
				break;
			case 5:
				x += 1;
				break;
			}
		}
	}

	// create some walls
	for (size_t i = 0; i < w * h; ++i)
	{
		int x = i / h;
		int y = i % h;

		if (tilemap[x][y])
		{
			Entity entity;
			entity.p = Vec2(x, y) * tilemap.tile_size;

			Sprite sprite("tile.png");
			entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

			engine.add_entity(std::move(entity));
		}
	}

	{
		Entity entity;
		
		Collider collider;
		collider.shape = std::make_unique<TilemapShape>(&tilemap);
		entity.addComponent(engine.cs->colliders.add(std::move(collider)));

		engine.add_entity(std::move(entity));
	}

	// create fps counter
	{
		Entity entity;

		auto fps = std::make_shared<FrameRate>();
		entity.addComponent(&**engine.cbs->behaviours.add(fps));

		engine.add_entity(std::move(entity));
	}

	engine.run();

	return 0;
}