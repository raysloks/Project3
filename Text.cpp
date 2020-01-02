#include "Text.h"

#include <thread>
#include <fstream>

Text::Text(const std::string & fname)
{
	loaded = false;

	std::thread t([this, fname]()
		{
			std::ifstream in(fname, std::ios::in | std::ios::binary);
			if (in)
			{
				std::string buffer;
				in.seekg(0, std::ios::end);
				buffer.resize(in.tellg());
				in.seekg(0, std::ios::beg);
				in.read(&buffer[0], buffer.size());
				in.close();
				swap(buffer);
			}
			loaded = true;
		});
	t.detach();
}
