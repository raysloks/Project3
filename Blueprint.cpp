#include "Blueprint.h"

#include <thread>
#include <sstream>

#include "Text.h"

#include "Vec3.h"

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
				std::istringstream is(*text);
				
				uint32_t entity_count;
				is >> entity_count;

				for (size_t i = 0; i < entity_count; ++i)
				{
					uint32_t parent_index;
					is >> parent_index;
					Vec3 position;
					is >> position.x >> position.y >> position.z;
				}

				uint32_t component_count;
				is >> component_count;
			}
			
			bp->loaded = true;
		});
	t.detach();

	return bp;
}

Reference<Entity> Blueprint::build(Level & level) const
{
	return Reference<Entity>();
}
