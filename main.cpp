#include "Engine.h"

#include "Player.h"
#include "Enemy.h"
#include "SpriteAnimator.h"
#include "BouncingBall.h"

#include "FrameRate.h"
#include "HealthDisplay.h"
#include "FollowCursor.h"

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
	engine.input->keyBindings.set(KB_ACTION_4, SDLK_5);
	engine.input->keyBindings.set(KB_ACTION_5, SDLK_6);
	engine.input->keyBindings.set(KB_ACTION_6, SDLK_7);
	engine.input->keyBindings.set(KB_ACTION_7, SDLK_8);
	engine.input->keyBindings.set(KB_ACTION_8, SDLK_9);
	engine.input->keyBindings.set(KB_ACTION_9, SDLK_0);

	// TODO remove after a better option to keep a resource loaded is added
	auto swoop = SpriteSheet::get("swoop_small.png");

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

	auto floor = SpriteSheet::get("floor.png");

	auto floor_iso_gen_lossy = floor->makeIsometricFloorLossy(false);
	auto floor_iso_gen_lossy_blur = floor->makeIsometricFloorLossy(true);
	auto floor_iso_gen_lossless = floor->makeIsometricFloorLossless();

	SpriteSheet::resources.insert(std::make_pair("floor_iso_gen_lossy", floor_iso_gen_lossy));
	SpriteSheet::resources.insert(std::make_pair("floor_iso_gen_lossy_blur", floor_iso_gen_lossy_blur));
	SpriteSheet::resources.insert(std::make_pair("floor_iso_gen_lossless", floor_iso_gen_lossless));

	const int w = 64;
	const int h = 64;

	Tilemap tilemap(w, h);
	tilemap.tile_size = Vec2(16.0f, 16.0f);

	for (int x = 0; x < w; ++x)
	{
		for (int y = 0; y < h; ++y)
		{
			tilemap[x][y].tile = (rand() % 16 << 1) | 1;
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
			tilemap[x][y].tile &= ~1;

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
	tileset->offset_y = -8;
	tileset->columns = 4;
	tileset->rows = 8;

	// create a sprite for each tile
	// should probably be replaced by some sort of tile rendering system
	for (size_t x = 0; x < w; ++x)
	{
		for (size_t y = 0; y < h; ++y)
		{
			auto& tile = tilemap[x][y];
			bool show_floor = true;
			if (tile.tile & 1)
			{
				show_floor = false;

				int walls = 0;
				if (tilemap.at(x + 1, y + 1).tile & 1)
					walls += 1;
				if (tilemap.at(x, y + 1).tile & 1)
					walls += 1;
				if (tilemap.at(x - 1, y + 1).tile & 1)
					walls += 1;
				if (tilemap.at(x - 1, y).tile & 1)
					walls += 1;
				if (tilemap.at(x - 1, y - 1).tile & 1)
					walls += 1;
				if (tilemap.at(x, y - 1).tile & 1)
					walls += 1;
				if (tilemap.at(x + 1, y - 1).tile & 1)
					walls += 1;
				if (tilemap.at(x + 1, y).tile & 1)
					walls += 1;

				if (walls < 8)
				{
					if (tile.tile & 32)
						show_floor = true;

					Entity entity;
					entity.xy = Vec2(x, y) * tilemap.tile_size;

					Sprite sprite("tile_iso.png");
					size_t tile_index = (tile.tile >> 1);
					sprite.subsprite_x = tile_index % sprite.sheet->columns;
					sprite.subsprite_y = tile_index / sprite.sheet->columns;
					entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

					engine.add_entity(std::move(entity));
				}
			}
			if (show_floor)
			{
				Entity entity;
				entity.xy = Vec2(x, y) * tilemap.tile_size;

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
		
		auto collider = engine.cs->tilemaps.add(TilemapCollider(&tilemap));
		collider->layers = 1 | 4;
		entity.addComponent(collider);

		engine.add_entity(std::move(entity));
	}

	// create player
	{
		Entity entity;
		entity.x = 16;
		entity.y = 16 * (h - 2);

		auto sprite = SpriteSheet::get("bone_boy.png");
		sprite->columns = 12;
		sprite->rows = 2;
		sprite->offset_y = -8;

		entity.addComponent(engine.srs->sprites.add(Sprite(sprite)));

		auto player = std::make_shared<Player>();
		player->tm = &tilemap;
		entity.addComponent(engine.cbs->add(player));

		entity.addComponent(engine.cs->circles.add(CircleCollider(3.0f)));

		auto player_entity = engine.add_entity(std::move(entity));

		// create shadow
		{
			Entity entity;
			player_entity->addChild(&entity);

			Sprite sprite("shadow4_iso.png");
			sprite.sort = -64;
			sprite.color = SDL_Color({ 0, 0, 0, 32 });
			entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

			engine.add_entity(std::move(entity));
		}

		// create health display
		{
			Entity entity;

			auto display = std::make_shared<HealthDisplay>();
			display->player = player;
			entity.addComponent(engine.cbs->add(display));

			engine.add_entity(std::move(entity));
		}
	}

	// create fps counter
	{
		Entity entity;

		auto fps = std::make_shared<FrameRate>();
		entity.addComponent(engine.cbs->add(fps));

		engine.add_entity(std::move(entity));
	}

	// cursor highlight
	{
		Entity entity;

		Sprite sprite("shadow6_iso.png");
		entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

		entity.addComponent(engine.cbs->add(std::make_shared<FollowCursor>()));

		engine.add_entity(std::move(entity));
	}

	// create ghost
	if (false)
	{
		Entity entity;
		entity.x = 16;
		entity.y = 16 * (h - 20);

		Sprite sprite("ghost.png");
		sprite.sheet->columns = 25;
		sprite.sort = 128;
		sprite.color.a = 200;
		sprite.sheet->offset_y = -8;
		entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

		auto animator = std::make_shared<SpriteAnimator>(10.0f);
		entity.addComponent(engine.cbs->add(std::move(animator)));

		auto enemy = std::make_shared<Enemy>();
		entity.addComponent(engine.cbs->add(enemy));

		CircleCollider collider(6.0f);
		collider.layers = 2;
		entity.addComponent(engine.cs->circles.add(std::move(collider)));

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
			entity.addComponent(engine.cbs->add(std::move(animator)));

			engine.add_entity(std::move(entity));
		}

		{
			Entity entity;
			enemy_entity->addChild(&entity);

			Sprite sprite("shadow6_iso.png");
			sprite.sort = -64;
			sprite.color = SDL_Color({ 0, 0, 0, 32 });
			entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

			engine.add_entity(std::move(entity));
		}
	}

	// create enemy
	for (int i = 0; i < 5; ++i)
	{
		Entity entity;
		entity.x = 16 * (5 + i * 2);
		entity.y = 16 * (h - 10);

		Sprite sprite("imp.png");
		sprite.sheet->columns = 4;
		sprite.sheet->rows = 2;
		sprite.sheet->offset_y = -8;
		entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

		auto enemy = std::make_shared<Enemy>();
		entity.addComponent(engine.cbs->add(enemy));

		CircleCollider collider(3.0f);
		entity.addComponent(engine.cs->circles.add(std::move(collider)));

		auto enemy_entity = engine.add_entity(std::move(entity));

		{
			Entity entity;
			enemy_entity->addChild(&entity);

			Sprite sprite("shadow4_iso.png");
			sprite.sort = -64;
			sprite.color = SDL_Color({ 0, 0, 0, 32 });
			entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

			engine.add_entity(std::move(entity));
		}
	}

	// create bouncing ball
	{
		Entity entity;
		entity.x = 16 * 16;
		entity.y = 16 * (h - 16);
		entity.z = 25;

		Sprite sprite("ball.png");
		entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

		auto bouncing_ball = std::make_shared<BouncingBall>();
		bouncing_ball->v = Vec2(25.0f, 25.0f);
		entity.addComponent(engine.cbs->add(bouncing_ball));

		entity.addComponent(engine.cs->circles.add(CircleCollider(5.0f)));

		engine.add_entity(std::move(entity));

		{
			Entity entity;

			bouncing_ball->shadow = engine.srs->sprites.add(Sprite("shadow6_iso.png"));
			bouncing_ball->shadow->color = SDL_Color({ 0, 0, 0, 32 });
			bouncing_ball->shadow->sort = -64;
			entity.addComponent(bouncing_ball->shadow);

			engine.add_entity(std::move(entity));
		}
	}

	// create stairs
	{
		Entity entity;
		entity.x = 16 * 4;
		entity.y = 16 * (h - 4);

		Sprite sprite("stairs.png");
		sprite.sheet->offset_y = -8;
		entity.addComponent(engine.srs->sprites.add(std::move(sprite)));

		{
			Entity child;
			child.x = 8.0f;

			child.addComponent(engine.cs->rectangles.add(RectangleCollider(Vec2(0.0f, 8.0f))));

			entity.addChild(&child);

			engine.add_entity(std::move(child));
		}

		{
			Entity child;
			child.x = -8.0f;

			child.addComponent(engine.cs->rectangles.add(RectangleCollider(Vec2(0.0f, 8.0f))));

			entity.addChild(&child);

			engine.add_entity(std::move(child));
		}

		{
			Entity child;
			child.y = -8.0f;

			child.addComponent(engine.cs->rectangles.add(RectangleCollider(Vec2(8.0f, 0.0f))));

			entity.addChild(&child);

			engine.add_entity(std::move(child));
		}

		engine.add_entity(std::move(entity));
	}

	engine.run();

	return 0;
}