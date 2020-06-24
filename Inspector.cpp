#include "Inspector.h"

#include "Diamond.h"

#include <sstream>

void Inspector::start()
{
	font = SpriteSheet::get("font.png")->makeOutline({ 0, 0, 0, 255 }, { 255, 255, 255, 255 });;
	font->rows = 16;
	font->columns = 16;

	index = 0;

	TextDisplay::start();
}

#include <iostream>

void Inspector::tick(float dt)
{
	if (input->isKeyDown(SDLK_LALT))
	{
		if (input->isKeyPressed(SDLK_PAGEDOWN))
		{
			Reference<Entity> entity;
			while (true)
			{
				++index;
				entity = level->get_entity(index);
				if (entity == nullptr)
				{
					index = 0;
					break;
				}
				if (entity->getComponents().size() > 1)
					break;
			}
		}
	}
	else
	{
		if (input->isKeyPressed(SDLK_PAGEDOWN))
			++index;
		if (input->isKeyPressed(SDLK_PAGEUP))
			--index;
	}

	std::stringstream ss;

	{
		auto entity = level->get_entity(index);
		if (entity != nullptr)
		{
			ss << entity->x << " " << entity->y << " " << entity->z << "\n";
			for (auto&& component : entity->getComponents())
			{
				Diamond::print(ss, component.get());
			}
		}
	}

	setText(ss.str());

	entity->x = 32;
	entity->y = 32;
}
