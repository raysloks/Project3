#pragma once

#include "Resource.h"

#include <mutex>

#include <ft2build.h>
#include FT_FREETYPE_H

class SpriteSheet;
class Vec2;

class Font :
	public Resource<Font>
{
public:
	Font();
	~Font();

	static std::shared_ptr<Font> load(const std::string& fname);

	std::shared_ptr<SpriteSheet> getGlyph(intmax_t code, const Vec2& size);
	std::shared_ptr<SpriteSheet> getAtlas(const Vec2& size, float outline = 0.0f);

private:
	FT_Face ftFace;
	std::string buffer;
	std::mutex mutex;
};

