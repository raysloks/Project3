#include "FloatingText.h"

FloatingText::FloatingText(const std::string & text, float speed, float duration) : text(text), v(0.0f, 0.0f, speed), duration(duration)
{
	time = 0.0f;
}

void FloatingText::start()
{
	auto font = SpriteSheet::get("font.png")->makeOutline({ 0, 0, 0, 255 }, { 255, 255, 255, 255 });
	font->rows = 16;
	font->columns = 16;

	for (size_t i = 0; i < text.size(); ++i)
	{
		Entity entity;
		this->entity->addChild(&entity);
		entity.x -= text.size() * 0.5f;
		entity.x += i + 0.5f;
		entity.x *= 4;
		entity.y = -entity.x;

		auto sprite = srs->sprites.add(Sprite(font));

		sprite->sort = 512;
		//sprite->scale = 0.5f;

		size_t c = text[i];
		sprite->subsprite_x = c % 16;
		sprite->subsprite_y = c / 16;

		entity.addComponent(sprite);

		engine->add_entity(std::move(entity));
	}
}

void FloatingText::tick(float dt)
{
	entity->xyz += v * dt;

	for (auto child : entity->getChildren())
		child->getComponent<Sprite>()->color.a = 255 - 255 * time / duration;

	time += dt;
	if (time > duration)
		engine->remove_entity(entity);
}
