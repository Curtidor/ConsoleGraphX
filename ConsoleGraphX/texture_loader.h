#pragma once
#include <string>
#include "texture.h"
#include <utility>
#include <cstdint>
#include <iosfwd>

namespace ConsoleGraphX_Internal
{
	class TextureLoader
	{
	public:
		static Texture* LoadTexture(const std::string& fileName);

	private:
		static Texture* _AllocateAndReadTexture(std::ifstream& textureFile, uint32_t width, uint32_t height);

		static std::pair<uint32_t, uint32_t> _ReadTextureDimensions(std::ifstream& textureFile);

		static void _VerifyFileSize(std::ifstream& textureFile, uint32_t width, uint32_t height);


	};
};
