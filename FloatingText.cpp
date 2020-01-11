#include "FloatingText.h"

FloatingText::FloatingText(const std::string & text, float speed, float duration) : text(text), speed(speed), duration(duration)
{
	time = 0.0f;
}

void FloatingText::start()
{
	auto font = SpriteSheet::get("font.png");
	font->rows = 16;
	font->columns = 16;

	for (size_t i = 0; i < text.size(); ++i)
	{
		Entity entity;
		this->entity->addChild(&entity);
		entity.p.x -= text.size() * 0.5f;
		entity.p.x += i + 0.5f;
		entity.p.x *= 4;
		entity.p.y = -entity.p.x;

		auto sprite = srs->sprites.add(Sprite(font));

		sprite->sort = 512;

		size_t c = text[i];
		sprite->subsprite_x = c % 16;
		sprite->subsprite_y = c / 16;

		entity.addComponent(sprite);

		engine->add_entity(std::move(entity));
	}
}

void FloatingText::tick(float dt)
{
	entity->p.x -= dt * speed;
	entity->p.y -= dt * speed;

	time += dt;
	if (time > duration)
		engine->remove_entity(entity);
}
