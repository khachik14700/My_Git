#pragma once
#include <filesystem>

class Repository
{
    public:
        static bool init(const std::filesystem::path& path);
        static bool exists(const std::filesystem::path& path);
        static bool isValid(const std::filesystem::path& path);
};