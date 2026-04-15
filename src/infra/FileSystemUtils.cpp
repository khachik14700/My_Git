#include "FileSystemUtils.h"
#include <fstream>

bool FileSystemUtils::exists(const std::filesystem::path& path)
{
    return std::filesystem::exists(path);
}

bool FileSystemUtils::isDirectory(const std::filesystem::path& path)
{
    return std::filesystem::is_directory(path);
}

bool FileSystemUtils::isRegularFile(const std::filesystem::path& path)
{
    return std::filesystem::is_regular_file(path);
}

bool FileSystemUtils::createDirectories(const std::filesystem::path& path)
{
    if (std::filesystem::exists(path))
    {
        return true;
    }
    return std::filesystem::create_directories(path);
}

bool FileSystemUtils::writeTextFile(const std::filesystem::path& path, const std::string& content)
{
    std::ofstream file(path);

    if (!file.is_open())
    {
        return false;
    }

    file << content;
    if (!file.good())
    {
        return false;
    }
    file.close();
    return true;
}