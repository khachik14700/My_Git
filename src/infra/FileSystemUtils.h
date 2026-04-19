#pragma once
#include <filesystem>
#include <string>

class FileSystemUtils
{
    public:
        static bool exists(const std::filesystem::path& path);
        static bool isDirectory(const std::filesystem::path& path);
        static bool isRegularFile(const std::filesystem::path& path);
        static bool createDirectories(const std::filesystem::path& path);
        static bool writeTextFile(const std::filesystem::path& path, const std::string& content);
        static bool writeBinaryFile(const std::filesystem::path& path, const std::string& content);
        static bool readBinaryFile(const std::filesystem::path& path, std::string& content);
};