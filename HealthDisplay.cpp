#include "HealthDisplay.h"

#include <chrono>

#include "Player.h"

#include "MobPosHandler.h"

#include "Window.h"

#include "ModelRenderSystem.h"

#include "Font.h"

void HealthDisplay::start()
{
	fonts.push_back(Font::get("RobotoMono-Regular.ttf")->getAtlas(Vec2(28.0f), 4.0f));
	fonts.push_back(Font::get("RobotoMono-Regular.ttf")->getAtlas(Vec2(28.0f), 1.0f));

	window.reset(new Window());
	bg.reset(new Window());
	fg.reset(new Window());
	text_windows.push_back(std::make_shared<Window>());
	text_windows.push_back(std::make_shared<Window>());

	window->addChild(bg);
	window->addChild(fg);
	mrs->ui->addChild(window);

	bg->model = std::make_shared<ModelRenderer>("offset_plane.mdl", "pixel.png", "", 1);
	bg->model->uniform_buffer_object.color = Vec4(0.01f, 0.5f);
	fg->model = std::make_shared<ModelRenderer>("offset_plane.mdl", "pixel.png", "", 1);
	fg->model->uniform_buffer_object.color = Vec4(Vec3(0.9f, 0.01f, 0.01f), 1.0f);

	for (auto& text_window : text_windows)
	{
		window->addChild(text_window);
		text_window->minAnchor = Vec2(0.5f, 0.0f);
		text_window->maxAnchor = Vec2(0.5f, 0.0f);
		text_window->maxOffset = Vec2(1.0f);
	}

	window->minAnchor = Vec2(0.5f, 0.0f);
	window->maxAnchor = Vec2(0.5f, 0.0f);

	window->minOffset = Vec2(-200.0f, 20.0f);
	window->maxOffset = Vec2(200.0f, 60.0f);
}

void HealthDisplay::tick(float dt)
{
	if (player == nullptr)
		return;

	for (auto& font : fonts)
		if (!font->loaded)
			return;

	int64_t current_hp = player->hp.evaluate(net->time);

	fg->maxAnchor = Vec2((float)current_hp / player->hp.cap, 1.0f);

	std::string lhs = format(current_hp);
	std::string rhs = format(player->hp.cap);
	std::string text = lhs + " / " + rhs;
	
	if (text != prev_text)
	{
		prev_text = text;
		for (size_t k = 0; k < fonts.size(); ++k)
		{
			auto& font = fonts[k];
			Vec2 offset;
			auto model = std::make_shared<Model>();
			model->vertices.resize(text.size() * 4);
			model->triangles.resize(text.size() * 2);
			for (size_t i = 0; i < text.size(); ++i)
			{
				unsigned char c = text[i];
				auto& frame = font->frames[c];

				model->vertices[i * 4 + 0].uv = frame.min;
				model->vertices[i * 4 + 1].uv = Vec2(frame.max.x, frame.min.y);
				model->vertices[i * 4 + 2].uv = frame.max;
				model->vertices[i * 4 + 3].uv = Vec2(frame.min.x, frame.max.y);
				model->vertices[i * 4 + 0].position = Vec2(frame.min.x, frame.max.y);
				model->vertices[i * 4 + 1].position = frame.max;
				model->vertices[i * 4 + 2].position = Vec2(frame.max.x, frame.min.y);
				model->vertices[i * 4 + 3].position = frame.min;
				for (size_t j = i * 4; j < i * 4 + 4; ++j)
				{
					auto& vertex = model->vertices[j];
					vertex.position -= frame.center;
					vertex.position *= Vec2(font->surface->w, font->surface->h);
					vertex.position += offset;
					vertex.normal = Vec3(0.0f, 0.0f, 1.0f);
					vertex.bones[0] = 0;
					vertex.bones[1] = 0;
					vertex.bones[2] = 0;
					vertex.bones[3] = 0;
					vertex.weights[0] = 1.0f;
					vertex.weights[1] = 0.0f;
					vertex.weights[2] = 0.0f;
					vertex.weights[3] = 0.0f;
				}

				model->triangles[i * 2 + 0].indices[0] = i * 4 + 1;
				model->triangles[i * 2 + 0].indices[1] = i * 4 + 0;
				model->triangles[i * 2 + 0].indices[2] = i * 4 + 2;
				model->triangles[i * 2 + 1].indices[2] = i * 4 + 3;
				model->triangles[i * 2 + 1].indices[0] = i * 4 + 2;
				model->triangles[i * 2 + 1].indices[1] = i * 4 + 0;

				offset += frame.advance;
			}
			model->loaded = true;

			auto& text_window = text_windows[k];
			text_window->model = std::make_shared<ModelRenderer>(model, font, nullptr, 1);
			text_window->minOffset = Vec2(0.0f, 8.0f) - getAdvance(lhs, font) - getAdvance(" / ", font) * 0.5f;
			text_window->maxOffset = text_window->minOffset + 1.0f;
		}
		text_windows[0]->model->uniform_buffer_object.color = Vec4(Vec3(0.0f), 1.0f);
	}
}

std::string HealthDisplay::format(int64_t number) const
{
	if (number < 10000)
		return std::to_string(number);
	return std::to_string(number / 1000) + "k";
}

std::string HealthDisplay::format(int64_t number, size_t right) const
{
	std::string text = format(number);
	while (text.size() < right)
		text = " " + text;
	return text;
}

Vec2 HealthDisplay::getAdvance(const std::string& text, const std::shared_ptr<SpriteSheet>& font) const
{
	Vec2 offset;
	for (size_t i = 0; i < text.size(); ++i)
	{
		unsigned char c = text[i];
		offset += font->frames[c].advance;
	}
	return offset;
}
