#include "TextArea.h"

#include "Font.h"
#include "ModelRenderer.h"

#include <iostream>

TextArea::TextArea()
{
	size = 12.0f;
	
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
		auto sheet = font->getAtlas(size);
		auto text_model = sheet->makeTextModel(text, settings);
		text_window_shared->model = std::make_shared<ModelRenderer>(text_model, sheet, nullptr, 1);
		text_window_shared->model->uniform_buffer_object.color = settings.color;
		text_window_shared->minOffset = -sheet->getJustifiedOffset(text, settings) * align + Vec2(0.0f, 1.0f - align.y) * size;
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
		if (text_window_shared->model && text_window_shared->model->texture)
			return text_window_shared->model->texture->getIndexFromPosition(text, settings, position - text_window_shared->getMin());
	}
	return 0;
}
