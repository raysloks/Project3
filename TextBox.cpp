#include "TextBox.h"

#include "MobPosHandler.h"

void TextBox::start()
{
	font = SpriteSheet::get("font.png")->makeOutline({ 0, 0, 0, 255 }, { 255, 255, 255, 255 });;
	font->rows = 16;
	font->columns = 16;

	active = false;

	input->addTextInputCallback([this](const std::string& text)
		{
			if (level->contains_entity(entity))
				if (active)
					setText(this->text + text);
		});
	input->addKeyDownCallback(SDLK_RETURN, [this]()
		{
			if (level->contains_entity(entity))
			{
				if (active && !text.empty())
				{
					MpChat message;
					message.message = text;
					net->Send(message);

					setText("");
				}
				active = !active;
			}
		});
	input->addUnfilteredKeyDownCallback(SDLK_BACKSPACE, [this]()
		{
			if (level->contains_entity(entity))
				if (active)
					setText(text.substr(0, text.size() - 1));
		});

	TextDisplay::start();
}
