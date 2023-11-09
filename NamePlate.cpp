#include "NamePlate.h"

#include "ModelRenderer.h"
#include "Font.h"
#include "TextArea.h"
#include "Border.h"

NamePlate::NamePlate()
{
	auto bg_locked = std::make_shared<Window>();
	auto fg_locked = std::make_shared<Window>();

	addChild(bg_locked);
	addChild(fg_locked);

	bg_locked->model = std::make_shared<ModelRenderer>("offset_plane.mdl", "pixel.png", "", 1);
	bg_locked->model->uniform_buffer_object.color = Vec4(0.01f, 0.5f);
	fg_locked->model = std::make_shared<ModelRenderer>("offset_plane.mdl", "pixel.png", "", 1);
	fg_locked->model->uniform_buffer_object.color = Vec4(Vec3(0.9f, 0.01f, 0.01f), 1.0f);

	bg = bg_locked;
	fg = fg_locked;

	for (size_t i = 0; i < 2; ++i)
	{
		auto text_window_locked = std::make_shared<TextArea>();
		addChild(text_window_locked);
		text_window_locked->minAnchor = Vec2(0.5f, 0.0f);
		text_window_locked->maxAnchor = Vec2(0.5f, 0.0f);
		text_window_locked->maxOffset = Vec2(1.0f);
		text_window_locked->font = Font::get("RobotoMono-Regular.ttf");
		text_window_locked->outline = (i == 0) ? 4.0f : 1.0f;
		text_window_locked->settings.color = (i == 0) ? Vec4(Vec3(0.0f), 1.0f) : Vec4(Vec3(1.0f), 1.0f);
		text_windows.push_back(text_window_locked);
	}

	auto border = std::make_shared<Border>(0.0f, 2.0f);
	border->setColor(Vec4(Vec3(0.0f), 1.0f));
	addChild(border);
}

void NamePlate::setName(const std::string & name)
{
}

void NamePlate::setHealth(int health)
{
	health_current = health;
	updateHealthText();
}

void NamePlate::setMaxHealth(int max_health)
{
	health_max = max_health;
	//updateHealthText();
}

void NamePlate::updateHealthText()
{
	std::string text = std::to_string(health_current) + " / " + std::to_string(health_max);
	if (text != last_text)
	{
		for (auto& text_window : text_windows)
		{
			auto text_window_locked = text_window.lock();
			text_window_locked->text = text;
			text_window_locked->update();
		}
	}
	last_text = text;
}
