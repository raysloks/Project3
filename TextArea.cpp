#include "TextArea.h"

#include "Font.h"
#include "ModelRenderer.h"

#include <iostream>

TextArea::TextArea()
{
	size = 12.0f;
	outline = 0.0f;
	
	settings.line_height = 14.0f;
	settings.justification = Vec2(0.0f, 0.0f);
	settings.color = Vec4(1.0f, 1.0f);

	auto text_window_shared = std::make_shared<Window>();
	addChild(text_window_shared);
	text_window = text_window_shared;
}

void TextArea::update()
{
	auto text_window_shared = text_window.lock();
	if (font && text_window_shared)
	{
		if (atlas == nullptr)
			atlas = font->getAtlas(size, outline);
		auto text_model = atlas->makeTextModel(text, settings);
		if (text_window_shared->model == nullptr)
		{
			text_window_shared->model = std::make_shared<ModelRenderer>(text_model, atlas, nullptr, 1);
			text_window_shared->model->uniform_buffer_object.color = settings.color;
		}
		else
		{
			text_window_shared->model->model = text_model;
			text_window_shared->model->textures = { atlas };
			text_window_shared->model->rendering_model = nullptr;
			text_window_shared->model->setDirty();
		}
		text_window_shared->minOffset = -atlas->getJustifiedOffset(text, settings) * align + Vec2(0.0f, 1.0f - align.y) * size;
		text_window_shared->maxOffset = text_window_shared->minOffset + 1.0f;
		text_window_shared->minAnchor = align;
		text_window_shared->maxAnchor = align;
	}
}

int64_t TextArea::getIndexFromPosition(const Vec2& position) const
{
	auto text_window_shared = text_window.lock();
	if (text_window_shared)
	{
		if (text_window_shared->model && text_window_shared->model->textures.size())
			return text_window_shared->model->textures[0]->getIndexFromPosition(text, settings, position - text_window_shared->getMin());
	}
	return 0;
}
