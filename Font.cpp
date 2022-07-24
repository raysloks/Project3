#include "Font.h"

#include <thread>
#include <mutex>
#include <fstream>

#include <freetype/ftstroke.h>

#include "SpriteSheet.h"
#include "Model.h"

#include "Vec2.h"

FT_Library ftLibrary = nullptr;
std::mutex ftLibraryMutex;

Font::Font()
{
	ftFace = nullptr;
	loaded = false;
}

Font::~Font()
{
	std::lock_guard<std::mutex> guard(ftLibraryMutex);
	if (ftFace)
		FT_Done_Face(ftFace);
}

std::shared_ptr<Font> Font::load(const std::string& fname)
{
	{
		std::lock_guard<std::mutex> guard(ftLibraryMutex);
		if (ftLibrary == nullptr)
		{
			FT_Error ftError = FT_Init_FreeType(&ftLibrary);
			if (ftError)
			{
			}
		}
	}

	auto font = std::make_shared<Font>();
	auto func = [font, fname]()
	{
		std::ifstream in("data/" + fname, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			font->buffer.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&font->buffer[0], font->buffer.size());
			in.close();

			std::lock_guard<std::mutex> guard(ftLibraryMutex);
			FT_New_Memory_Face(ftLibrary, (const FT_Byte*)font->buffer.data(), font->buffer.size(), 0, &font->ftFace);
		}

		font->loaded = true;
	};

	thread_pool.push(func);

	return font;
}

std::shared_ptr<SpriteSheet> Font::getGlyph(intmax_t code, const Vec2& size)
{
	auto sheet = std::make_shared<SpriteSheet>();
	auto shared_this = shared_from_this();
	auto func = [shared_this, sheet, code, size]()
	{
		while (!shared_this->loaded)
			std::this_thread::yield();

		std::lock_guard<std::mutex> guard(shared_this->mutex);

		FT_Set_Pixel_Sizes(shared_this->ftFace, (FT_UInt)size.x, (FT_UInt)size.y);
		FT_UInt index = FT_Get_Char_Index(shared_this->ftFace, code);
		FT_Load_Glyph(shared_this->ftFace, index, 0);
		FT_Render_Glyph(shared_this->ftFace->glyph, FT_RENDER_MODE_NORMAL);

		auto& bitmap = shared_this->ftFace->glyph->bitmap;

		sheet->surface = SDL_CreateRGBSurface(0, bitmap.width + 2, bitmap.rows + 2, 32, 0xff, 0xff << 8, 0xff << 16, 0xff << 24);
		intmax_t pitch = sheet->surface->pitch;
		uint8_t * pixels = (uint8_t*)sheet->surface->pixels;
		for (size_t y = 0; y < sheet->surface->h; ++y)
		{
			for (size_t x = 0; x < sheet->surface->w; ++x)
			{
				SDL_Color& pixel = *(SDL_Color*)(pixels + y * pitch + x * 4);
				if (x > 0 && y > 0 && x < (size_t)sheet->surface->w - 1 && y < (size_t)sheet->surface->h - 1)
					pixel = { 255, 255, 255, bitmap.buffer[(y - 1) * bitmap.pitch + (x - 1)] };
				else
					pixel = { 255, 255, 255, 0 };
			}
		}

		sheet->loaded = true;
	};

	if (loaded)
		func();
	else
		thread_pool.push(func);

	return sheet;
}

std::shared_ptr<SpriteSheet> Font::getAtlas(const Vec2& size, float outline)
{
	auto sheet = std::make_shared<SpriteSheet>();
	auto shared_this = shared_from_this();
	auto func = [shared_this, sheet, size, outline]()
	{
		while (!shared_this->loaded)
			std::this_thread::yield();

		sheet->surface = SDL_CreateRGBSurface(0, 2048, 2048, 32, 0xff, 0xff << 8, 0xff << 16, 0xff << 24);

		std::lock_guard<std::mutex> guard(shared_this->mutex);
		std::lock_guard<std::mutex> library_guard(ftLibraryMutex);

		FT_Set_Pixel_Sizes(shared_this->ftFace, (FT_UInt)size.x, (FT_UInt)size.y);

		std::vector<FT_UInt> indices;
		std::vector<FT_ULong> codes;

		FT_UInt index = 0;
		FT_ULong code = FT_Get_First_Char(shared_this->ftFace, &index);
		if (index == 0)
			code = FT_Get_Next_Char(shared_this->ftFace, code, &index);
		while (index != 0)
		{
			indices.push_back(index);
			codes.push_back(code);
			code = FT_Get_Next_Char(shared_this->ftFace, code, &index);
		}

		size_t fill_x = 0;
		size_t fill_y = 0;
		size_t max_glyph_height_on_row = 0;

		FT_Stroker stroker;
		FT_Stroker_New(ftLibrary, &stroker);
		FT_Stroker_Set(stroker, (1 << 6) * outline, FT_STROKER_LINECAP_SQUARE, FT_STROKER_LINEJOIN_MITER, 2 << 16);

		for (size_t i = 0; i < indices.size(); ++i)
		{
			auto& index = indices[i];
			auto& code = codes[i];

			FT_Load_Glyph(shared_this->ftFace, index, 0);

			FT_Glyph glyph;
			FT_Get_Glyph(shared_this->ftFace->glyph, &glyph);
			if (outline > 0.0f)
				FT_Glyph_StrokeBorder(&glyph, stroker, false, true);
			FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, nullptr, true);
			FT_BitmapGlyph bitmap_glyph = reinterpret_cast<FT_BitmapGlyph>(glyph);

			auto& bitmap = bitmap_glyph->bitmap;

			size_t pixel_width = 1;
			size_t real_width = bitmap.width / pixel_width + 1;

			if (fill_x + real_width > sheet->surface->w)
			{
				fill_x = 0;
				fill_y += max_glyph_height_on_row + 1;
				max_glyph_height_on_row = bitmap.rows;
			}

			intmax_t pitch = sheet->surface->pitch;
			uint8_t * pixels = (uint8_t*)sheet->surface->pixels + fill_y * pitch + fill_x * 4;
			for (size_t y = 0; y < bitmap.rows; ++y)
			{
				for (size_t x = 0; x < bitmap.width; x += pixel_width)
				{
					SDL_Color& pixel = *(SDL_Color*)(pixels + y * pitch + x / pixel_width * 4);
					pixel = { 255, 255, 255, bitmap.buffer[y * bitmap.pitch + x]};
				}
			}

			Vec2 normalize_multiplier = Vec2(1.0f) / Vec2(sheet->surface->w, sheet->surface->h);

			sheet->frames[code] = {
				Vec2(fill_x, fill_y) * normalize_multiplier,
				Vec2(fill_x + real_width, fill_y + bitmap.rows + 1) * normalize_multiplier,
				(Vec2(fill_x, fill_y) + Vec2(-bitmap_glyph->left, (intmax_t)bitmap.rows + 1 - bitmap_glyph->top)) * normalize_multiplier,
				Vec2(bitmap_glyph->root.advance.x >> 16, bitmap_glyph->root.advance.y >> 16)
			};

			fill_x += real_width;
			max_glyph_height_on_row = std::max(max_glyph_height_on_row, (size_t)bitmap.rows);

			FT_Done_Glyph(glyph);
		}

		FT_Stroker_Done(stroker);

		sheet->loaded = true;
	};

	if (loaded)
		func();
	else
		thread_pool.push(func);

	return sheet;
}
