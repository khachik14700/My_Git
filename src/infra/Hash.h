#pragma once
#include <string>

class Hash
{
    public:
        static std::string sha256(const std::string& data);
        static std::string hexToBytes(const std::string& hex);
};