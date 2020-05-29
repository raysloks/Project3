#include "TextBox.h"

void TextBox::start()
{
	font = SpriteSheet::get("font.png")->makeOutline({ 0, 0, 0, 255 }, { 255, 255, 255, 255 });;
	font->rows = 16;
	font->columns = 16;

	input->addTextInputCallback([this](const std::string& text)
		{
			if (level->contains_entity(entity))
				setText(this->text + text);
		});
	input->addUnfilteredKeyDownCallback(SDLK_BACKSPACE, [this]()
		{
			if (level->contains_entity(entity))
				setText(text.substr(0, text.size() - 1));
		});

	TextDisplay::start();
}
