#pragma once

#include "Resource.h"

#include <SDL.h>

#include <vulkan/vulkan.h>

#include "Vec2.h"

class ModelRenderSystem;
class Model;

class SpriteSheet :
	public Resource<SpriteSheet>
{
public:
	SpriteSheet();
	SpriteSheet(size_t w, size_t h);
	~SpriteSheet();

	SpriteSheet(const SpriteSheet &) = delete;
	SpriteSheet(SpriteSheet &&) = delete;

	static std::shared_ptr<SpriteSheet> load(const std::string & fname);

	static void destroyUnusedTextures();

	void save(const std::string & fname) const;

	static std::shared_ptr<SpriteSheet> createIsometricFloorLosslessMap(size_t w, size_t h);

	std::shared_ptr<SpriteSheet> merge(const std::shared_ptr<SpriteSheet>& fg) const;

	std::shared_ptr<SpriteSheet> makeCopy() const;

	std::shared_ptr<SpriteSheet> makeMapped(const std::shared_ptr<SpriteSheet> & map) const;

	std::shared_ptr<SpriteSheet> makeIsometricWall() const;

	std::shared_ptr<SpriteSheet> makeIsometricFloorLossy(bool blur) const;
	std::shared_ptr<SpriteSheet> makeIsometricFloorLossless(float rotation = 0.0f) const;

	std::shared_ptr<SpriteSheet> makeScaled(intmax_t scale) const;

	std::shared_ptr<SpriteSheet> makeOutline(const SDL_Color & outline_color = SDL_Color({ 255, 255, 255, 255 }), const SDL_Color & fill_color = SDL_Color({ 0, 0, 0, 0 }), intmax_t cell_margin = 1, intmax_t outline_width = 1) const;

	std::shared_ptr<Model> makeTextModel(const std::string& text) const;
	std::shared_ptr<Model> makeFrameModel(uint32_t frame_index, bool normalize = false) const;

	SDL_Texture * getTexture(SDL_Renderer * render);

	void createTexture(ModelRenderSystem * mrs);
	void createTextureImage(ModelRenderSystem * mrs);
	void createTextureImageView(ModelRenderSystem * mrs);
	void createTextureSampler(ModelRenderSystem * mrs);

	struct Frame
	{
		Vec2 min, max, center, advance;
	};

//private:
	SDL_Surface * surface;
	SDL_Texture * texture;

	size_t rows, columns;
	int offset_x, offset_y;

	std::map<uint32_t, Frame> frames;

	VkImage texture_image;
	VkDeviceMemory texture_image_memory;
	VkImageView texture_image_view;
	VkSampler texture_sampler;
	VkImageLayout texture_image_layout;
};

