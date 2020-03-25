#include "FloatingText.h"

FloatingText::FloatingText(const std::string & text, float speed, float duration) : text(text), v(0.0f, 0.0f, speed), duration(duration), color(1.0f)
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
		auto entity = level->add_entity();
		Entity::adopt(entity, this->entity);
		entity->x -= text.size() * 0.5f;
		entity->x += i + 0.5f;
		entity->x *= 4;
		entity->y = -entity->x;

		auto sprite = level->sprites.add(font);

		sprite->sort = 512;
		//sprite->scale = 0.5f;

		size_t c = text[i];
		sprite->subsprite_x = c % 16;
		sprite->subsprite_y = c / 16;

		Component::attach(sprite, entity);
	}
}

void FloatingText::tick(float dt)
{
	entity->xyz += (v + a * dt * 0.5f) * dt;
	v += a * dt;

	for (auto child : entity->getChildren())
		child->getComponent<Sprite>()->color = SDL_Color({ uint8_t(color.x * 255), uint8_t(color.y * 255), uint8_t(color.z * 255), uint8_t(255 - 255 * time / duration) });

	time += dt;
	if (time > duration)
		level->remove_entity(entity);
}
