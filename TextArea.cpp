#include "TextArea.h"

#include "Font.h"
#include "ModelRenderer.h"

void TextArea::update()
{
	if (font)
	{
		model->texture = font->getAtlas(16.0f);
	}
}
