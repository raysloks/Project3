#include "Border.h"

#include "ModelRenderer.h"

Border::Border(float inner_width, float outer_width)
{
	for (size_t i = 0; i < 8; ++i)
		addChild(std::make_shared<Window>());
	for (auto& child : children)
		child->model = std::make_shared<ModelRenderer>("offset_plane.mdl", "pixel.png", "", 1);
	setWidth(inner_width, outer_width);
}

void Border::setWidth(float inner_width, float outer_width)
{
	children[0]->setAnchorOffset(Vec2(0.0f, 1.0f), Vec2(0.0f, 1.0f), Vec2(-outer_width, -inner_width), Vec2(inner_width, outer_width));
	children[1]->setAnchorOffset(Vec2(0.0f, 1.0f), Vec2(1.0f, 1.0f), Vec2(inner_width, -inner_width), Vec2(-inner_width, outer_width));
	children[2]->setAnchorOffset(Vec2(1.0f, 1.0f), Vec2(1.0f, 1.0f), Vec2(-inner_width, -inner_width), Vec2(outer_width, outer_width));
	children[3]->setAnchorOffset(Vec2(1.0f, 0.0f), Vec2(1.0f, 1.0f), Vec2(-inner_width, inner_width), Vec2(outer_width, -inner_width));
	children[4]->setAnchorOffset(Vec2(1.0f, 0.0f), Vec2(1.0f, 0.0f), Vec2(-inner_width, -outer_width), Vec2(outer_width, inner_width));
	children[5]->setAnchorOffset(Vec2(0.0f, 0.0f), Vec2(1.0f, 0.0f), Vec2(inner_width, -outer_width), Vec2(-inner_width, inner_width));
	children[6]->setAnchorOffset(Vec2(0.0f, 0.0f), Vec2(0.0f, 0.0f), Vec2(-outer_width, -outer_width), Vec2(inner_width, inner_width));
	children[7]->setAnchorOffset(Vec2(0.0f, 0.0f), Vec2(0.0f, 1.0f), Vec2(-outer_width, inner_width), Vec2(inner_width, -inner_width));
}

void Border::setColor(const Vec4& color)
{
	for (auto& child : children)
		child->model->uniform_buffer_object.color = color;
}
