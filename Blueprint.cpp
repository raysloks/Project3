#include "Blueprint.h"

#include <thread>
#include <sstream>

#include "Text.h"

#include "Coal.h"

#include "Diamond.h"

Blueprint::Blueprint()
{
}

Blueprint::Blueprint(Reference<Entity> entity)
{
}

std::shared_ptr<Blueprint> Blueprint::load(const std::string & fname)
{
	auto bp = std::make_shared<Blueprint>();
	std::thread t([bp, fname]()
		{
			auto text = Text::get(fname);

			while (!text->loaded)
				std::this_thread::yield();

			if (text->size())
			{
				std::map<std::string, std::pair<Reference<Entity>, const Coal&>> entities;
				std::map<std::string, std::pair<Reference<Component>, const Coal&>> components;

				std::stringstream ss(*text);

				auto coal = Coal::parse(ss);
				if (coal)
				{
					if (coal->type == Coal::Type::Object)
					{
						for (auto&& member : coal->members)
						{
							if (member.second.type == Coal::Type::Array)
							{
								if (member.second.elements.size() == 2)
								{
									std::string type = member.second.elements[0].string;
									if (type == "Entity")
									{
										auto entity = bp->level.add_entity();
										entities.emplace(std::make_pair(member.first, std::make_pair(entity, member.second.elements[1])));
									}
									else
									{
										Reference<Component> component;
										auto index = Diamond::index(type);
										if (index == std::type_index(typeid(CircleCollider)))
											component = bp->level.circle_colliders.add();
										if (index == std::type_index(typeid(RectangleCollider)))
											component = bp->level.rectangle_colliders.add();
										if (index == std::type_index(typeid(SpriteCollider)))
											component = bp->level.sprite_colliders.add();
										if (index == std::type_index(typeid(TilemapCollider)))
											component = bp->level.tilemap_colliders.add();
										if (index == std::type_index(typeid(Sprite)))
											component = bp->level.sprites.add();
										components.emplace(std::make_pair(member.first, std::make_pair(component, member.second.elements[1])));
									}
								}
							}
						}

						for (auto&& [guid, entity] : entities)
						{
							entity.first->x = entity.second.members.at("x").real;
							entity.first->y = entity.second.members.at("y").real;
							entity.first->z = entity.second.members.at("z").real;
							auto parent = entity.second.members.find("parent");
							if (parent != entity.second.members.end())
								Entity::adopt(entity.first, entities.at(parent->second.string).first);
						}

						for (auto&& [guid, component] : components)
						{
							Diamond::init(component.first.get(), component.second);
						}
					}
				}
			}
			
			bp->loaded = true;
		});
	t.detach();

	return bp;
}

void Blueprint::save(const std::string & fname) const
{
	auto shared_this = shared_from_this();
	auto func = [shared_this, fname]()
	{
		while (!shared_this->loaded)
			std::this_thread::yield();

		Coal coal;
		for (auto entity : shared_this->level.entities.components)
		{
			std::string guid;

			Coal data({
				{ "x", entity.x },
				{ "y", entity.y },
				{ "z", entity.z } });

			auto parent = entity.getParent();
			if (parent)
				data.members["parent"] = Coal();

			coal.members.emplace(std::make_pair(guid, Coal({ "Entity", std::move(data) })));

			for (auto component : entity.getComponents())
			{
			}
		}
	};

	if (loaded)
	{
		func();
	}
	else
	{
		std::thread t(func);
		t.detach();
	}
}

Reference<Entity> Blueprint::build(Level & level) const
{
	return Reference<Entity>();
}
