#include "Engine.h"

#include "Player.h"
#include "Enemy.h"
#include "SpriteAnimator.h"

#include "FrameRate.h"

#include "Tilemap.h"
#include "TilemapShape.h"

#include "GameKeyBinding.h"

#include "Text.h"

int main(int argc, char* args[])
{
	Engine engine;

	engine.input->keyBindings.set(KB_ATTACK, SDLK_f);
	engine.input->keyBindings.set(KB_DASH, SDLK_SPACE);

	engine.input->keyBindings.set(KB_UP, SDLK_w);
	engine.input->keyBindings.set(KB_LEFT, SDLK_a);
	engine.input->keyBindings.set(KB_DOWN, SDLK_s);
	engine.input->keyBindings.set(KB_RIGHT, SDLK_d);

	// create player
	{
		Entity entity;
		entity.x = 16;
		entity.y = 16;

		auto sprite = SpriteSheet::get("dude_one.png");
		sprite->columns = 4;
		sprite->rows = 2;
		entity.addComponent(engine.srs->sprites.add(Sprite(sprite)));

		auto player = std::make_shared<Player>();
		entity.addComponent(&**engine.cbs->behaviours.add(player));

		Collider collider;
		collider.shape = std::make_unique<Circle>(6.0f);
		entity.addComponent(engine.cs->colliders.add(std::move(collider)));

		auto player_entity_index = engine.add_entity(std::move(entity));

		{
			Entity entity;
			engine.get_entity(player_entity_index)->addChild(&entity);

			Sprite sprite("shadow.png");
			sprite.sort = -16;
			entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

			engine.add_entity(std::move(entity));
		}
	}

	// create enemy
	{
		Entity entity;
		entity.x = 100;
		entity.y = 100;

		Sprite sprite("ghost.png");
		sprite.sheet->columns = 24;
		sprite.sort = 32;
		sprite.color.a = 200;
		entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

		auto animator = std::make_shared<SpriteAnimator>(16.0f);
		entity.addComponent(&**engine.cbs->behaviours.add(std::move(animator)));

		auto enemy = std::make_shared<Enemy>();
		entity.addComponent(&**engine.cbs->behaviours.add(enemy));

		Collider collider;
		collider.shape = std::make_unique<Circle>(6.0f);
		collider.layers = 2;
		entity.addComponent(engine.cs->colliders.add(std::move(collider)));

		auto enemy_entity_index = engine.add_entity(std::move(entity));

		{
			Entity entity;
			engine.get_entity(enemy_entity_index)->addChild(&entity);

			Sprite sprite("shadow.png");
			sprite.sort = -16;
			entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

			engine.add_entity(std::move(entity));
		}
	}

	/*for (int x = 0; x < 40; ++x)
	{
		for (int y = 0; y < 40; ++y)
		{
			Entity entity;
			entity.p = Vec2(x, y) + Vec2(50, 50);

			Sprite sprite("pixel.png");
			sprite.color = SDL_Color{ 0, 0, 0, 255 };
			entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

			Collider collider;
			collider.shape = std::make_unique<Circle>(0.5f);
			collider.layers = 2;
			entity.addComponent(engine.cs->colliders.add(std::move(collider)));

			engine.add_entity(std::move(entity));
		}
	}*/

	// create test animation
	{
		Entity entity;
		entity.x = 50;
		entity.y = 50;

		Sprite sprite("key_glimmer.png");
		sprite.sheet->columns = 4;
		sprite.sheet->rows = 4;
		sprite.sort = -8;
		entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

		auto animator = std::make_shared<SpriteAnimator>(15.0f);
		entity.addComponent(&**engine.cbs->behaviours.add(std::move(animator)));

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

	// create cave-like paths
	for (size_t i = 0; i < 8; ++i)
	{
		int x = 0;
		int y = 0;
		for (size_t i = 0; i < 600; ++i)
		{
			if (x < 1)
				x = 1;
			if (y < 1)
				y = 1;
			if (x > w - 2)
				x = w - 2;
			if (y > h - 2)
				y = h - 2;
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
			case 4:
				y += 1;
				break;
			case 5:
				x += 1;
				break;
			}
		}
	}

	// create a sprite for each tile
	// should probably be replaced by some sort of tile rendering system
	for (size_t i = 0; i < w * h; ++i)
	{
		size_t x = i / h;
		size_t y = i % h;

		if (tilemap[x][y])
		{
			int walls = 4;
			if (y != 0 && !tilemap[x][y - 1])
				walls -= 1;
			if (x != 0 && !tilemap[x - 1][y])
				walls -= 1;
			if (y != h - 1 && !tilemap[x][y + 1])
				walls -= 1;
			if (x != w - 1 && !tilemap[x + 1][y])
				walls -= 1;

			if (walls < 4)
			{
				Entity entity;
				entity.p = Vec2(x, y) * tilemap.tile_size;

				Sprite sprite("tile.png");
				entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

				engine.add_entity(std::move(entity));
			}
		}
		else
		{
			Entity entity;
			entity.p = Vec2(x, y) * tilemap.tile_size;

			Sprite sprite("floor.png");
			sprite.sort = -128;
			entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

			engine.add_entity(std::move(entity));
		}
	}

	// create tilemap collider
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