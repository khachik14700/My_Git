#include "GitIgnore.h"
#include "FileSystemUtils.h"

#ifdef __linux__
    #include <unistd.h>
#elif defined(__APPLE__)
    #include <mach-o/dyld.h>
#elif defined(_WIN32)
    #include <windows.h>
#endif

GitIgnore::GitIgnore(const std::filesystem::path& repo_root) : repo_path(repo_root) {}

static std::string trim(const std::string& str)
{
    std::size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos)
    {
        return "";
    }
    std::size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool GitIgnore::load()
{
    std::filesystem::path path = repo_path / ".mygitignore";
    if (!FileSystemUtils::exists(path))
    {
        return true;
    }
    std::string content = "";
    if (!FileSystemUtils::readBinaryFile(path, content))
    {
        throw std::runtime_error("Error: failed to read .mygitignore");
    }
    std::size_t pos = 0;

    while (pos < content.size())
    {
        std::size_t newline_pos = content.find('\n', pos);
        if (newline_pos == std::string::npos)
        {
            newline_pos = content.size();
        }
        std::string line = content.substr(pos, newline_pos - pos);
        pos = newline_pos + 1;

        line = trim(line);
        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        patterns.push_back(line);
    }

    return true;
}

bool GitIgnore::isIgnored(const std::filesystem::path& path) const
{
    std::string relative = std::filesystem::relative(path, repo_path).string();
    std::string filename = path.filename().string();
    for (const std::string& pattern : patterns)
    {
        if (relative == pattern || filename == pattern)
        {
            return true;
        }
    }
    return false;
}

bool GitIgnore::isOwnExecutable(const std::filesystem::path& path)
{
    if (!FileSystemUtils::exists(path) || FileSystemUtils::isDirectory(path))
    {
        return false;
    }

    try
    {
        std::filesystem::path selfPath;
#ifdef __linux__
        char buf[1024] = {0};
        ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
        if (len != -1)
        {
            buf[len] = '\0';
            selfPath = buf;
        }

#elif defined(__APPLE__)
        char buf[1024] = {0};
        uint32_t size = sizeof(buf);
        if (_NSGetExecutablePath(buf, &size) == 0)
        {
            selfPath = buf;
            if (selfPath.is_relative())
                selfPath = std::filesystem::absolute(selfPath);
        }

#elif defined(_WIN32)
        char buf[MAX_PATH] = {0};
        if (GetModuleFileNameA(NULL, buf, MAX_PATH) != 0)
        {
            selfPath = buf;
        }
#endif

        if (selfPath.empty())
            return false;

        return std::filesystem::equivalent(path, selfPath);
    }
    catch(...)
    {
        return false;
    }
}