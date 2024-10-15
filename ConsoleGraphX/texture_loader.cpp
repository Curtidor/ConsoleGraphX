#include "CGXPCH.h"
#include <iosfwd>
#include <fstream>
#include <cstdint>
#include "texture.h"
#include "file_utils.h"
#include "texture_loader.h"

namespace ConsoleGraphX_Internal
{
    Texture* TextureLoader::LoadTexture(const std::string& filename)
    {
        VerifyFileExtension(filename, ".cxsp");

        std::ifstream textureFile = OpenFile(filename);

        auto [width, height] = _ReadTextureDimensions(textureFile);

        // verify the file size matches expectations
        _VerifyFileSize(textureFile, width, height);

        Texture* texture = _AllocateAndReadTexture(textureFile, width, height);

        textureFile.close();
        return texture;
    }

    Texture* TextureLoader::_AllocateAndReadTexture(std::ifstream& textureFile, uint32_t width, uint32_t height)
    {
        Texture* texture = new Texture(width, height);

        const std::size_t expectedPixelDataSize = static_cast<size_t>(width) * height * sizeof(CHAR_INFO);
        textureFile.read(reinterpret_cast<char*>(texture->GetPixels()), expectedPixelDataSize);

        if (textureFile.fail())
        {
            delete texture;
            throw std::runtime_error("Failed to read texture data");
        }

        return texture;
    }

    std::pair<uint32_t, uint32_t> TextureLoader::_ReadTextureDimensions(std::ifstream& textureFile)
    {
        uint32_t width = 0, height = 0;
        textureFile.read(reinterpret_cast<char*>(&width), 4);
        textureFile.read(reinterpret_cast<char*>(&height),4);

        if (textureFile.fail())
        {
            throw std::runtime_error("Failed to read texture dimensions");
        }

        return { width, height };
    }

    void TextureLoader::_VerifyFileSize(std::ifstream& textureFile, uint32_t width, uint32_t height)
    {
        std::size_t expectedPixelDataSize = static_cast<size_t>(width) * height * sizeof(CHAR_INFO);
        std::streampos currentPosition = textureFile.tellg();

        textureFile.seekg(0, std::ios_base::end);
        std::streamsize fileSize = textureFile.tellg();
        textureFile.seekg(currentPosition);  // return to the position after reading width and height

        if (fileSize != (static_cast<std::size_t>(currentPosition) + expectedPixelDataSize))
        {
            throw std::runtime_error("File size does not match expected size");
        }
    }
};