#include "PCH_CGX.h"
#include <iosfwd>
#include <fstream>
#include <cstdint>
#include "Engine\Graphics\texture.h"
#include "Engine\Core\Utils\file_utils.h"
#include "Engine\Resources\texture_loader.h"
#include "WinCore.h"

namespace ConsoleGraphX_Internal
{
    Texture* TextureLoader::LoadTexture(const std::string& filename)
    {
        VerifyFileExtension(filename, ".cxsp");

        std::ifstream textureFile = OpenFile(filename);

        TextureInfo tInfo{};
        _ReadTextureHeader(tInfo, textureFile);

        // verify the file size matches expectations
        _VerifyFileSize(textureFile, tInfo.width, tInfo.height);

        Texture* texture = _AllocateAndReadTexture(textureFile, tInfo);

        textureFile.close();
        return texture;
    }

    Texture* TextureLoader::_AllocateAndReadTexture(std::ifstream& textureFile, TextureInfo& tInfo)
    {
        Texture* texture = new Texture(tInfo.width, tInfo.height, tInfo.id);

        const std::size_t expectedPixelDataSize = static_cast<size_t>(tInfo.width) * tInfo.height * sizeof(CHAR_INFO);
        textureFile.read(reinterpret_cast<char*>(texture->GetPixels()), expectedPixelDataSize);

        if (textureFile.fail())
        {
            delete texture;
            throw std::runtime_error("Failed to read texture data");
        }

        return texture;
    }

    void TextureLoader::_ReadTextureHeader(TextureInfo& tInfo, std::ifstream& textureFile)
    {
        uint32_t VERSION = 0;
        uint32_t SUPPORTED_VERSION = 1;
        textureFile.read(reinterpret_cast<char*>(&VERSION), 4);

        if (VERSION != SUPPORTED_VERSION) 
        {
            throw std::runtime_error("UnSupported Texture Version");
        }

        textureFile.read(reinterpret_cast<char*>(&tInfo.width), 4);
        textureFile.read(reinterpret_cast<char*>(&tInfo.height), 4);
        textureFile.read(reinterpret_cast<char*>(&tInfo.id),4);

        if (textureFile.fail())
        {
            throw std::runtime_error("Failed to read texture dimensions");
        }
    }

    void TextureLoader::_VerifyFileSize(std::ifstream& textureFile, uint32_t width, uint32_t height)
    {
        std::size_t expectedPixelDataSize = static_cast<size_t>(width) * height * sizeof(CHAR_INFO);
        std::streampos currentPosition = textureFile.tellg();

        textureFile.seekg(0, std::ios_base::end);
        std::streamsize fileSize = textureFile.tellg();
        textureFile.seekg(currentPosition);  // return to the position after reading width and height

        if (fileSize != (static_cast<long long>(currentPosition) + expectedPixelDataSize))
        {
            throw std::runtime_error("File size does not match expected size");
        }
    }
};