#include "CGXPCH.h"
#include <filesystem>
#include <fstream>
#include "file_utils.h"

void VerifyFileExtension(const std::string& filename, const std::string& expectedExtension)
{
    std::filesystem::path filePath(filename);

    if (!filePath.has_extension() || filePath.extension() != expectedExtension)
    {
        throw std::runtime_error("Invalid file extension");
    }
}

std::ifstream OpenFile(const std::string& filename)
{
    std::ifstream textureFile(filename, std::ios_base::binary);
    if (!textureFile.is_open())
    {
        throw std::runtime_error("Failed to open file");
    }

    return textureFile;
}
