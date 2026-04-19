#pragma once
#include <string>

class Compression
{
    public:
        static std::string compress(const std::string& data);
        static std::string decompress(const std::string& data);
};