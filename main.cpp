#include "Engine.h"

#include "Player.h"
#include "Enemy.h"
#include "SpriteAnimator.h"
#include "BouncingBall.h"

#include "FrameRate.h"

#include "Tilemap.h"
#include "TilemapShape.h"

#include "GameKeyBinding.h"

#include "Text.h"

int main(int argc, char* args[])
{
	Engine engine;

	engine.input->keyBindings.set(KB_UP, SDLK_w);
	engine.input->keyBindings.set(KB_LEFT, SDLK_a);
	engine.input->keyBindings.set(KB_DOWN, SDLK_s);
	engine.input->keyBindings.set(KB_RIGHT, SDLK_d);

	engine.input->keyBindings.set(KB_ACTION_0, SDLK_1);
	engine.input->keyBindings.set(KB_ACTION_1, SDLK_2);
	engine.input->keyBindings.set(KB_ACTION_2, SDLK_3);
	engine.input->keyBindings.set(KB_ACTION_3, SDLK_4);

	// TODO remove after a better option to keep a resource loaded is added
	auto swoop = SpriteSheet::get("swoop.png");

	// create bouncing ball
	{
		Entity entity;
		entity.x = 16 * 2;
		entity.y = 16 * (100 - 2);

		Sprite sprite("shadow5_iso.png");
		sprite.sort = -64;
		sprite.color = SDL_Color({ 0, 0, 0, 64 });
		entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

		auto bouncing_ball = std::make_shared<BouncingBall>();
		bouncing_ball->v = Vec2(25.0f, 25.0f);
		bouncing_ball->z = 25.0f;
		entity.addComponent(&**engine.cbs->behaviours.add(bouncing_ball));

		Collider collider;
		collider.shape = std::make_unique<Circle>(5.0f);
		entity.addComponent(engine.cs->colliders.add(std::move(collider)));

		engine.add_entity(std::move(entity));

		{
			Entity entity;

			bouncing_ball->visual = engine.srs->sprites.add(Sprite("ball.png"));
			entity.addComponent(bouncing_ball->visual);

			engine.add_entity(std::move(entity));
		}
	}

	// create enemy
	{
		Entity entity;
		entity.x = 100;
		entity.y = 100;

		Sprite sprite("ghost.png");
		sprite.sheet->columns = 25;
		sprite.sort = 100;
		sprite.color.a = 200;
		sprite.sheet->offset_y = -8;
		entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

		auto animator = std::make_shared<SpriteAnimator>(10.0f);
		entity.addComponent(&**engine.cbs->behaviours.add(std::move(animator)));

		auto enemy = std::make_shared<Enemy>();
		entity.addComponent(&**engine.cbs->behaviours.add(enemy));

		Collider collider;
		collider.shape = std::make_unique<Circle>(6.0f);
		collider.layers = 2;
		entity.addComponent(engine.cs->colliders.add(std::move(collider)));

		auto enemy_entity = engine.add_entity(std::move(entity));

		{
			Entity entity;
			enemy_entity->addChild(&entity);

			Sprite sprite("ghost_hands.png");
			sprite.sheet->columns = 3;
			sprite.sheet->offset_x = 2;
			sprite.sheet->offset_y = -8;
			sprite.sort = 110;
			sprite.color.a = 200;
			entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

			auto animator = std::make_shared<SpriteAnimator>(1.0f);
			entity.addComponent(&**engine.cbs->behaviours.add(std::move(animator)));

			engine.add_entity(std::move(entity));
		}

		{
			Entity entity;
			enemy_entity->addChild(&entity);

			Sprite sprite("shadow6_iso.png");
			sprite.sort = -64;
			sprite.color = SDL_Color({ 0, 0, 0, 64 });
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

	// create key
	{
		Entity entity;
		entity.x = 16 * 2;
		entity.y = 16 * (100 - 3);

		Sprite sprite("key_glimmer.png");
		sprite.sheet->columns = 4;
		sprite.sheet->rows = 4;
		sprite.sort = -8;
		entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

		auto animator = std::make_shared<SpriteAnimator>(15.0f);
		entity.addComponent(&**engine.cbs->behaviours.add(std::move(animator)));

		engine.add_entity(std::move(entity));
	}

	auto floor = SpriteSheet::get("floor.png");

	auto floor_iso_gen_lossy = floor->makeIsometricFloorLossy(false);
	auto floor_iso_gen_lossy_blur = floor->makeIsometricFloorLossy(true);
	auto floor_iso_gen_lossless = floor->makeIsometricFloorLossless();

	SpriteSheet::resources.insert(std::make_pair("floor_iso_gen_lossy", floor_iso_gen_lossy));
	SpriteSheet::resources.insert(std::make_pair("floor_iso_gen_lossy_blur", floor_iso_gen_lossy_blur));
	SpriteSheet::resources.insert(std::make_pair("floor_iso_gen_lossless", floor_iso_gen_lossless));

	const int w = 100;
	const int h = 100;

	Tilemap tilemap(w, h);
	tilemap.tile_size = Vec2(16.0f, 16.0f);

	for (int x = 0; x < w; ++x)
	{
		for (int y = 0; y < h; ++y)
		{
			tilemap[x][y].tile = (rand() % 4 << 1) | 1;
		}
	}

	// create cave-like paths
	int head_x = 1;
	int head_y = h - 1;
	for (size_t i = 0; i < w + h; ++i)
	{
		int x = head_x;
		int y = head_y;
		int prev_dir = -1;

		switch (rand() % 2)
		{
		case 0:
			head_x += 1;
			break;
		case 1:
			head_y -= 1;
			break;
		}

		int length_category = rand() % 20 + 1;

		int length = 0;
		if (length_category > 1)
			length = rand() % 5;
		if (length_category == 19)
			length = rand() % 100 + 50;
		if (length_category == 20)
			length = rand() % 200 + 100;


		for (size_t i = 0; i <= length; ++i)
		{
			if (x < 1)
				x = 1;
			if (y < 1)
				y = 1;
			if (x > w - 2)
				x = w - 2;
			if (y > h - 2)
				y = h - 2;
			tilemap[x][y].tile = 0;
			if (rand() % 16 == 0)
			{
				size_t prevalence = rand() % 32 + 1ull;
				for (size_t i = 0; i < 16 * 16; ++i)
				{
					tilemap[x][y].effects.set(i / 16, i % 16, rand() % prevalence);
				}
			}

			int dir = rand() % 4;
			if (dir == (prev_dir + 2) % 4)
				dir = prev_dir;
			prev_dir = dir;
			switch (dir)
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
			}
		}
	}

	auto tileset = SpriteSheet::get("tile_iso.png");
	tileset->offset_y = -4;
	tileset->columns = 4;
	tileset->rows = 4;

	// create a sprite for each tile
	// should probably be replaced by some sort of tile rendering system
	for (size_t x = 0; x < w; ++x)
	{
		for (size_t y = 0; y < h; ++y)
		{
			auto& tile = tilemap[x][y];
			if (tile.tile)
			{
				int walls = 0;
				if (tilemap.at(x + 1, y + 1).tile)
					walls += 1;
				if (tilemap.at(x, y + 1).tile)
					walls += 1;
				if (tilemap.at(x - 1, y + 1).tile)
					walls += 1;
				if (tilemap.at(x - 1, y).tile)
					walls += 1;
				if (tilemap.at(x - 1, y - 1).tile)
					walls += 1;
				if (tilemap.at(x, y - 1).tile)
					walls += 1;
				if (tilemap.at(x + 1, y - 1).tile)
					walls += 1;
				if (tilemap.at(x + 1, y).tile)
					walls += 1;

				if (walls < 8)
				{
					Entity entity;
					entity.p = Vec2(x, y) * tilemap.tile_size;

					Sprite sprite("tile_iso.png");
					size_t tile_index = (tile.tile >> 1) % 4;
					sprite.subsprite_x = tile_index % sprite.sheet->columns;
					sprite.subsprite_y = tile_index / sprite.sheet->columns;
					entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

					engine.add_entity(std::move(entity));
				}
			}
			else
			{
				Entity entity;
				entity.p = Vec2(x, y) * tilemap.tile_size;

				auto sheet = SpriteSheet::get("floor_iso_gen_lossless");

				Sprite sprite(sheet);
				sprite.sort = -256;
				entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

				engine.add_entity(std::move(entity));

				if (!tile.effects.empty())
				{
					tile.refreshEffectSprite(Vec2(x, y) * tilemap.tile_size);
				}
			}
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

	// create player
	{
		Entity entity;
		entity.x = 16;
		entity.y = 16 * (100 - 2);

		auto sprite = SpriteSheet::get("dude_one.png");
		sprite->columns = 4;
		sprite->rows = 2;
		sprite->offset_y = -8;
		entity.addComponent(engine.srs->sprites.add(Sprite(sprite)));

		auto player = std::make_shared<Player>();
		player->tm = &tilemap;
		entity.addComponent(&**engine.cbs->behaviours.add(player));

		Collider collider;
		collider.shape = std::make_unique<Circle>(5.0f);
		entity.addComponent(engine.cs->colliders.add(std::move(collider)));

		auto player_entity = engine.add_entity(std::move(entity));

		{
			Entity entity;
			player_entity->addChild(&entity);

			Sprite sprite("shadow5_iso.png");
			sprite.sort = -64;
			sprite.color = SDL_Color({ 0, 0, 0, 64 });
			entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

			engine.add_entity(std::move(entity));
		}
	}

	engine.run();

	return 0;
}