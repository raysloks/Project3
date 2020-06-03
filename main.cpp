#include "Engine.h"

#include "Player.h"
#include "Enemy.h"
#include "SpriteAnimator.h"
#include "BouncingBall.h"

#include "FrameRate.h"
#include "HealthDisplay.h"
#include "FollowCursor.h"
#include "ActionBar.h"
#include "Inventory.h"
#include "Inspector.h"
#include "TextBox.h"

#include "Tilemap.h"

#include "GameKeyBinding.h"

#include "Text.h"

#include "Xoroshiro128Plus.h"

#include <chrono>

#include <iostream>

std::map<int, Level*> levels;

Level * create_level(int floor)
{
	Level * level = new Level();

	levels[floor] = level;

	level->rng.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());

	auto & tilemap = level->tilemap;
	intmax_t w = tilemap.getWidth();
	intmax_t h = tilemap.getHeight();

	Xoroshiro128Plus rng;
	rng.seed(floor);

	for (int x = 0; x < w; ++x)
	{
		for (int y = 0; y < h; ++y)
		{
			tilemap[x][y].tile = (rng.next(15) << 1) | 1;
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

		switch (rng.next(2))
		{
		case 0:
			head_x += 1;
			break;
		case 1:
			head_y -= 1;
			break;
		}

		int length_category = rng.next(1, 20);

		int length = 0;
		if (length_category > 1)
			length = rng.next(5);
		if (length_category == 19)
			length = rng.next(100) + 50;
		if (length_category == 20)
			length = rng.next(200) + 100;

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

			int dir = rng.next(3);
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

	int up_x = 4;
	int down_x = 2;
	if (floor % 2)
		std::swap(up_x, down_x);

	tilemap.at(up_x, h - 4).tile = 980;
	tilemap.at(down_x, h - 4).tile = 990;

	// create level change trigger
	{
		auto entity = level->add_entity();
		entity->x = 16 * down_x;
		entity->y = 16 * (h - 4) + 7;

		auto collider = level->rectangle_colliders.add(Vec2(7.0f, 0.0f));
		collider->callbacks.push_back([floor](const Collision & collision)
			{
				auto player = collision.other->getComponent<Player>();
				if (player)
				{
					auto level = levels[floor - 1];
					if (level == nullptr)
						level = create_level(floor - 1);
					player->level->move_entity(player->entity, level);
					player->engine->setLevel(level);
				}
			});
		Component::attach(collider, entity);
	}

	// create level change trigger
	{
		auto entity = level->add_entity();
		entity->x = 16 * up_x;
		entity->y = 16 * (h - 4) - 7;

		auto collider = level->rectangle_colliders.add(Vec2(7.0f, 0.0f));
		collider->callbacks.push_back([floor](const Collision & collision)
			{
				auto player = collision.other->getComponent<Player>();
				if (player)
				{
					auto level = levels[floor + 1];
					if (level == nullptr)
						level = create_level(floor + 1);
					player->level->move_entity(player->entity, level);
					player->engine->setLevel(level);
				}
			});
		Component::attach(collider, entity);
	}

	auto tileset = SpriteSheet::get("tile_iso.png");
	tileset->offset_y = -8;
	tileset->columns = 4;
	tileset->rows = 8;

	/*SpriteSheet::createIsometricFloorLosslessMap(16, 16)->save("test.png");

	SpriteSheet::get("splatter.png")->makeMapped(SpriteSheet::get("stairs_effect_map_two.png"))->save("test_mapped.png");

	SpriteSheet::get("floor.png")->makeIsometricWall()->save("test_wall.png");*/

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

					auto entity = level->add_entity();
					entity->xy = Vec2(x, y) * tilemap.getTileSize();

					auto sprite = level->sprites.add("tile_iso.png");
					size_t tile_index = (tile.tile >> 1);
					sprite->subsprite_x = tile_index % sprite->sheet->columns;
					sprite->subsprite_y = tile_index / sprite->sheet->columns;
					Component::attach(sprite, entity);
				}
			}

			if (tile.tile > 900)
			{
				auto entity = level->add_entity();
				entity->xy = Vec2(x, y) * tilemap.getTileSize();
				entity->z = tile.tile & 2 ? -16 : 0;

				auto sprite = level->sprites.add("stairs.png");
				sprite->sheet->offset_y = -8;
				Component::attach(sprite, entity);

				show_floor = false;
			}

			if (show_floor)
			{
				auto entity = level->add_entity();
				entity->xy = Vec2(x, y) * tilemap.getTileSize();

				auto sheet = SpriteSheet::get("floor_iso_gen_lossless");

				auto sprite = level->sprites.add(sheet);
				sprite->sort = -16;
				Component::attach(sprite, entity);

				if (!tile.effects.empty())
				{
					tile.refreshEffectSprite(Vec2(x, y) * tilemap.getTileSize());
				}
			}
		}
	}

	// create tilemap collider
	{
		auto entity = level->add_entity();

		auto collider = level->tilemap_colliders.add(&tilemap);
		collider->layers = 1 | 4;
		Component::attach(collider, entity);
	}

	// create fps counter
	{
		auto entity = level->add_entity();

		auto fps = level->add<FrameRate>();
		Component::attach(fps, entity);
	}

	// create enemy
	for (int i = 0; i < 5; ++i)
	{
		auto entity = level->add_entity();
		entity->x = 16 * (5 + i * 2);
		entity->y = 16 * (h - 10);

		auto sprite = level->sprites.add("imp.png");
		sprite->sheet->columns = 4;
		sprite->sheet->rows = 4;
		sprite->sheet->offset_y = -8;
		Component::attach(sprite, entity);

		auto enemy = level->add<Enemy>();
		Component::attach(enemy, entity);

		Component::attach(level->circle_colliders.add(3.0f), entity);

		auto enemy_entity = entity;

		{
			auto entity = level->add_entity();
			Entity::adopt(entity, enemy_entity);

			auto sprite = level->sprites.add("shadow4_iso.png");
			sprite->sort = 0;
			sprite->color = SDL_Color({ 0, 0, 0, 32 });
			Component::attach(sprite, entity);
		}
	}

	// create bouncing ball
	{
		auto entity = level->add_entity();
		entity->x = 16 * 16;
		entity->y = 16 * (h - 16);
		entity->z = 25;

		Component::attach(level->sprites.add("ball.png"), entity);

		auto bouncing_ball = level->add<BouncingBall>();
		bouncing_ball->v = Vec2(25.0f, 25.0f);
		bouncing_ball->elasticity = 1.0f;
		Component::attach(bouncing_ball, entity);

		Component::attach(level->circle_colliders.add(5.0f), entity);

		{
			auto entity = level->add_entity();

			bouncing_ball->shadow = level->sprites.add("shadow6_iso.png");
			bouncing_ball->shadow->color = SDL_Color({ 0, 0, 0, 32 });
			Component::attach(bouncing_ball->shadow, entity);
		}
	}

	// create pillar
	{
		auto entity = level->add_entity();
		entity->x = 16 * 16;
		entity->y = 16 * (h - 16);

		auto sprite = level->sprites.add("pillar.png");

		sprite->scale = Vec2(1.0f / 6.0f);

		Component::attach(sprite, entity);

		Component::attach(level->circle_colliders.add(6.0f), entity);
	}

	// cursor highlight
	{
		auto entity = level->add_entity();

		auto sprite = level->sprites.add("shadow6_iso.png");
		Component::attach(sprite, entity);

		auto highlight = level->add<FollowCursor>();
		Component::attach(highlight, entity);
	}

	// create ui
	{
		auto ui = level->add_entity();

		// create health display
		{
			auto entity = level->add_entity();
			Entity::adopt(entity, ui);

			auto display = level->add<HealthDisplay>();
			Component::attach(display, entity);
		}

		// create action bar
		{
			auto entity = level->add_entity();
			Entity::adopt(entity, ui);

			auto bar = level->add<ActionBar>();
			Component::attach(bar, entity);
		}

		// create inventory
		{
			auto entity = level->add_entity();
			Entity::adopt(entity, ui);

			auto inventory = level->add<Inventory>();
			Component::attach(inventory, entity);
		}

		// create inspector
		{
			auto entity = level->add_entity();
			Entity::adopt(entity, ui);

			auto inspector = level->add<Inspector>();
			Component::attach(inspector, entity);
		}

		// create text box
		{
			auto entity = level->add_entity();
			Entity::adopt(entity, ui);
			entity->xy = Vec2(32, 128);

			auto textBox = level->add<TextBox>();
			Component::attach(textBox, entity);
		}
	}

	return level;
}

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

	engine.input->keyBindings.set(KB_ACTION_1, -1);
	engine.input->keyBindings.set(KB_MOVE_CURSOR, -3);

	// TODO remove after a better option to keep a resource loaded is added
	auto swoop = SpriteSheet::get("swoop_small.png");
	auto stairs_effect_map = SpriteSheet::get("stairs_effect_map_two.png");
	stairs_effect_map->offset_y = -8;

	auto floor = SpriteSheet::get("floor.png");

	auto floor_iso_gen_lossy = floor->makeIsometricFloorLossy(false);
	auto floor_iso_gen_lossy_blur = floor->makeIsometricFloorLossy(true);
	auto floor_iso_gen_lossless = floor->makeIsometricFloorLossless();

	SpriteSheet::resources.insert(std::make_pair("floor_iso_gen_lossy", floor_iso_gen_lossy));
	SpriteSheet::resources.insert(std::make_pair("floor_iso_gen_lossy_blur", floor_iso_gen_lossy_blur));
	SpriteSheet::resources.insert(std::make_pair("floor_iso_gen_lossless", floor_iso_gen_lossless));

	Level * level = create_level(0);

	// create player
	{
		auto entity = level->add_entity();
		entity->x = 16;
		entity->y = 16 * (level->tilemap.getHeight() - 2);

		auto sheet = SpriteSheet::get("bone_boy.png");
		sheet->columns = 12;
		sheet->rows = 2;
		sheet->offset_y = -8;

		auto player = level->add<Player>();
		Component::attach(player, entity);

		Component::attach(level->circle_colliders.add(3.0f), entity);

		auto sprite = level->sprites.add(sheet);
		Component::attach(sprite, entity);

		auto player_entity = entity;

		// create shadow
		{
			auto entity = level->add_entity();
			Entity::adopt(entity, player_entity);

			auto sprite = level->sprites.add("shadow4_iso.png");
			sprite->sort = 0;
			sprite->color = SDL_Color({ 0, 0, 0, 32 });
			Component::attach(sprite, entity);
		}
	}

	engine.setLevel(level);

	engine.run();

	return 0;
}