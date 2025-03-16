#pragma once
#include <string>
#include <iosfwd>


void VerifyFileExtension(const std::string& filename, const std::string& expectedExtension);
std::ifstream OpenFile(const std::string& filename);

