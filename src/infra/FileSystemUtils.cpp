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
        file.close();
        return false;
    }
    file.close();
    return true;
}

bool FileSystemUtils::writeBinaryFile(const std::filesystem::path& path, const std::string& content)
{
    std::ofstream file(path, std::ios::binary | std::ios::out | std::ios::trunc);
    if (!file.is_open())
    {
        return false;
    }

    file.write(content.data(), content.size());
    if (!file.good())
    {
        file.close();
        return false;
    }
    file.close();
    return true;
}

bool FileSystemUtils::readBinaryFile(const std::filesystem::path& path, std::string& content)
{
    content.clear();

    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        return false;
    }

    const std::streamsize fileSize = file.tellg();
    if (fileSize < 0)
    {
        file.close();
        return false;
    }

    content.resize(static_cast<size_t>(fileSize));
    file.seekg(0, std::ios::beg);
    file.read(content.data(), fileSize);

    if (!file)
    {
        content.clear();
        file.close();
        return false;
    }

    file.close();
    return true;
}
