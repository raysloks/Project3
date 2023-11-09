#include "Text.h"

#include <thread>
#include <fstream>

std::shared_ptr<Text> Text::load(const std::string & fname)
{
	auto text = std::make_shared<Text>();
	text->loaded = false;

	std::thread t([text, fname]()
		{
			std::ifstream in(BaseResource::data_location + fname, std::ios::in | std::ios::binary);
			if (in)
			{
				std::string buffer;
				in.seekg(0, std::ios::end);
				buffer.resize(in.tellg());
				in.seekg(0, std::ios::beg);
				in.read(&buffer[0], buffer.size());
				in.close();
				text->swap(buffer);
			}

			text->loaded = true;
			text->loaded.notify_all();
		});
	t.detach();

	return text;
}
