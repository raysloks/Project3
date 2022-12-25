#include "NineSlice.h"

#include "SpriteSheet.h"
#include "ModelRenderer.h"

NineSlice::NineSlice(const std::shared_ptr<SpriteSheet>& sheet)
{
	Vec2 minAnchors[9] = {
		Vec2(0.0f, 1.0f),
		Vec2(0.0f, 1.0f),
		Vec2(1.0f, 1.0f),
		Vec2(0.0f, 0.0f),
		Vec2(0.0f, 0.0f),
		Vec2(1.0f, 0.0f),
		Vec2(0.0f, 0.0f),
		Vec2(0.0f, 0.0f),
		Vec2(1.0f, 0.0f)
	};

	Vec2 maxAnchors[9] = {
		Vec2(0.0f, 1.0f),
		Vec2(1.0f, 1.0f),
		Vec2(1.0f, 1.0f),
		Vec2(0.0f, 1.0f),
		Vec2(1.0f, 1.0f),
		Vec2(1.0f, 1.0f),
		Vec2(0.0f, 0.0f),
		Vec2(1.0f, 0.0f),
		Vec2(1.0f, 0.0f)
	};

	while (!sheet->loaded)
		std::this_thread::yield();

	Vec2 frameSizes[9];
	for (size_t i = 0; i < 9; ++i)
	{
		auto it = sheet->frames.find(i);
		if (it != sheet->frames.end())
			frameSizes[i] = it->second.max - it->second.min;
		else
			return;
	}

	Vec2 minOffsets[9] = {
		Vec2(0.0f, -frameSizes[0].y),
		Vec2(frameSizes[0].x, -frameSizes[1].y),
		-frameSizes[2],
		Vec2(0.0f, frameSizes[6].y),
		frameSizes[6],
		Vec2(-frameSizes[5].x, frameSizes[8].y),
		0.0f,
		Vec2(frameSizes[6].x, 0.0f),
		Vec2(-frameSizes[8].x, 0.0f)
	};

	Vec2 maxOffsets[9] = {
		Vec2(frameSizes[0].x, 0.0f),
		Vec2(-frameSizes[2].x, 0.0f),
		0.0f,
		Vec2(frameSizes[3].x, -frameSizes[0].y),
		-frameSizes[2],
		Vec2(0.0f, -frameSizes[2].y),
		frameSizes[6],
		Vec2(-frameSizes[8].x, frameSizes[7].y),
		Vec2(0.0f, frameSizes[8].y)
	};

	Vec2 sheet_size(sheet->surface->w, sheet->surface->h);
	for (size_t i = 0; i < 9; ++i)
	{
		auto child = std::make_shared<Window>(minAnchors[i], maxAnchors[i], minOffsets[i] * sheet_size, maxOffsets[i] * sheet_size);
		child->model = std::make_shared<ModelRenderer>(sheet->makeFrameModel(i, true), sheet, nullptr, 1);
		addChild(child);
	}
}
