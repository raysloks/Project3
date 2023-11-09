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

#include "ModelRenderSystem.h"
#include "MobPosHandler.h"

#include "Tilemap.h"

#include "GameKeyBinding.h"

#include "Text.h"
#include "Font.h"

#include "Xoroshiro128Plus.h"

#include "Ability.h"
#include "MobTemplate.h"

#include "Matrix3.h"

#include <chrono>

#include <iostream>

#include "TextEditArea.h"

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

	std::vector<uint8_t> mask_table(256, 255);
	std::vector<intmax_t> rotation_table(256);
	for (size_t i = 0; i < 256; ++i)
	{
		uint8_t mask = i;
		if (mask_table[mask] != 255)
			continue;
		for (size_t r = 0; r < 4; ++r)
		{
			size_t rotation = r * 2;
			uint8_t rotated_mask = (mask << rotation) | (mask >> (8 - rotation));
			mask_table[rotated_mask] = mask;
			rotation_table[rotated_mask] = r;
		}
	}

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

				int x_offsets[] = { -1, 0, 1, 1, 1, 0, -1, -1 };
				int y_offsets[] = { 1, 1, 1, 0, -1, -1, -1, 0 };

				int wall_mask = 0;
				for (size_t i = 0; i < 8; ++i)
					if (tilemap.at(x + x_offsets[i], y + y_offsets[i]).tile & 1)
						wall_mask |= 1 << i;

				if (wall_mask != 255)
				{
					if (tile.tile <= 900)
					{
						/*auto entity = level->add_entity();
						entity->xy = Vec2(x, y);

						auto model = level->models.add("offset_plane.mdl", "floor.png");
						Component::attach(model, entity);

						if (!tile.effects.empty())
						{
							tile.refreshEffectSprite(Vec2(x, y) * tilemap.getTileSize());
						}*/
					}

					/*auto entity = level->add_entity();
					entity->xy = Vec2(x, y) + 0.5f;

					std::string model_fname = "walls_test2.wall_" + std::to_string((uint64_t)mask_table[wall_mask]) + ".mdl";
					auto model = level->models.add(model_fname, "pixel.png");
					model->uniform_buffer_object.color = Vec4(0.8f, 0.4f, 0.3f, 1.0f);
					switch (rotation_table[wall_mask])
					{
					case 0:
						break;
					case 1:
						model->transform = Matrix3({
							0.0f, -1.0f, 0.0f,
							1.0f, 0.0f, 0.0f,
							0.0f, 0.0f, 1.0f
						});
						break;
					case 2:
						model->transform = Matrix3({
							-1.0f, 0.0f, 0.0f,
							0.0f, -1.0f, 0.0f,
							0.0f, 0.0f, 1.0f
						});
						break;
					case 3:
						model->transform = Matrix3({
							0.0f, 1.0f, 0.0f,
							-1.0f, 0.0f, 0.0f,
							0.0f, 0.0f, 1.0f
						});
						break;
					}
					Component::attach(model, entity);*/
				}
			}

			if (tile.tile > 900)
			{
				auto entity = level->add_entity();
				entity->xy = Vec2(x, y) + Vec2(1.5f, 0.5f);
				entity->z = tile.tile & 2 ? -0.5f : 0.5f;
				entity->scale = Vec3(0.5f);
				entity->rotation = Quaternion(M_PI / 2.0f, Vec3(0.0f, 0.0f, 1.0f));

				auto model = level->models.add("stone4.mdl", "pixel.png");
				model->uniform_buffer_object.color = Vec4(0.5f, 1.0f);
				Component::attach(model, entity);

				show_floor = false;
			}

			if (show_floor)
			{
				/*auto entity = level->add_entity();
				entity->xy = Vec2(x, y);

				auto model = level->models.add("offset_plane.mdl", "floor.png");
				Component::attach(model, entity);

				if (!tile.effects.empty())
				{
					tile.refreshEffectSprite(Vec2(x, y) * tilemap.getTileSize());
				}*/
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
	for (int i = 0; i < 0; ++i)
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
	if (false)
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
	if (false)
	{
		auto entity = level->add_entity();
		entity->x = 16.0f;
		entity->y = (h - 16.0f);

		auto model = level->models.add("walls_test2.wall_90.mdl", "wall.png");
		Component::attach(model, entity);
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
	if (true)
	{
		auto ui = level->add_entity();

		// create health display
		if (true)
		{
			auto entity = level->add_entity();
			Entity::adopt(entity, ui);

			auto display = level->add<HealthDisplay>();
			Component::attach(display, entity);
		}

		// create action bar
		if (true)
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
		if (false)
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

#include "Blueprint.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "ScriptCode.h"

#include <thread>

#include "Window.h"

#include "DiamondScript.h"

int main(int argc, char* args[])
{
	/*auto bp = Blueprint::load("test.bp");
	bp->save("data/test_save.bp");*/

	Ability::load();
	MobTemplate::load();

	Engine engine;

	size_t max_mem_size = 65536;
	void * mem = VirtualAlloc(nullptr, max_mem_size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	//while (true)
	//if (false)
	{
		//try
		{
			auto file = Text::get("text2.txt");
			while (!file->loaded)
				std::this_thread::yield();
			std::cout << *file << std::endl;

			auto iss = std::istringstream(*file);
			ScriptCode code(iss);

			ScriptCompile comp(mem);

			Diamond::populate(comp);

			for (auto i = code.statements.begin(); i != code.statements.end(); ++i)
			{
				std::cout << i->output() << std::endl;
				i->compile(comp);
			}

			comp.Compile();
			comp.Link();

			size_t mem_size = comp.ss.tellp();

			std::cout << "code bytes: " << mem_size << std::endl;

			comp.ss.read((char*)mem, mem_size);

			std::string class_name = "Main";
			auto found_class = comp.classes.find(class_name);
			if (found_class != comp.classes.end())
			{
				auto found_constructor = found_class->second->functions.find(class_name + "::" + class_name);
				if (found_constructor != found_class->second->functions.end())
				{
					Window * t = (Window*)operator new(found_class->second->size);
					memset(t, 0, found_class->second->size);

					void (*func)(Window*) = (void(*)(Window*))found_constructor->second.second;
					func(t);

					std::shared_ptr<Window> window(t);
					engine.mrs->ui->addChild(window);

					window->model = std::make_shared<ModelRenderer>(Model::load("offset_plane.mdl"), SpriteSheet::load("slot.png")->merge(SpriteSheet::load("splatter.png")), nullptr, 1);
				}
			}

		}
		/*catch (std::runtime_error& e)
		{
			std::cout << e.what() << std::endl;
		}*/
	}

	/*engine.input->keyBindings.set(KB_UP, SDLK_w);
	engine.input->keyBindings.set(KB_LEFT, SDLK_a);
	engine.input->keyBindings.set(KB_DOWN, SDLK_s);
	engine.input->keyBindings.set(KB_RIGHT, SDLK_d);*/

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

	//engine.input->keyBindings.set(KB_ACTION_1, -2);
	engine.input->keyBindings.set(KB_MOVE_ATTACK_CURSOR, std::numeric_limits<uint64_t>::max() - SDL_BUTTON_RIGHT);
	//engine.input->keyBindings.set(KB_ATTACK_CURSOR, -4);
	engine.input->keyBindings.set(KB_STOP_MOVE, SDLK_s);
	engine.input->keyBindings.set(KB_CANCEL_ACTION, SDLK_c);

	Level * level = create_level(0);

	engine.setLevel(level);

	engine.net->Connect();

	if (false)
	{
		auto text_edit_area = std::make_shared<TextEditArea>();
		text_edit_area->setSizeAnchorOffset(Vec2(200.0f, 200.0f));
		engine.mrs->ui->addChild(text_edit_area);
	}

	if (false)
	{
		auto ufo = std::make_shared<Window>();
		ufo->model = std::make_shared<ModelRenderer>(Model::load("offset_plane.mdl"), SpriteSheet::load("blurbustersUFOborder.png"), nullptr, 1);
		engine.mrs->ui->addChild(ufo);
		engine.ufo = ufo.get();
	}

	if (false)
	{
		auto testgrid = std::make_shared<Window>();
		testgrid->model = std::make_shared<ModelRenderer>(Model::load("offset_plane.mdl"), SpriteSheet::load("testgrid.png"), nullptr, 1);
		engine.mrs->ui->addChild(testgrid);
		engine.testgrid = testgrid.get();
	}

	engine.run();

	if (mem)
		VirtualFree(mem, 0, MEM_RELEASE);

	return 0;
}