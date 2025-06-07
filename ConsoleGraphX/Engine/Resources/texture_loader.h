#pragma once
#include <string>
#include "Engine\Graphics\texture.h"
#include <utility>
#include <cstdint>
#include <iosfwd>

namespace ConsoleGraphX_Internal
{

	struct TextureInfo
	{
		uint32_t width;
		uint32_t height;
		uint32_t id;
	};

	class TextureLoader
	{

	public:
		static Texture* LoadTexture(const std::string& fileName);

	private:
		static Texture* _AllocateAndReadTexture(std::ifstream& textureFile, TextureInfo& tInfo);

		static void _ReadTextureHeader(TextureInfo& tInfo, std::ifstream& textureFile);

		static void _VerifyFileSize(std::ifstream& textureFile, uint32_t width, uint32_t height);


	};
};
