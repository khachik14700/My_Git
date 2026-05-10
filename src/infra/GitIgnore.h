#pragma once
#include <filesystem>
#include <string>
#include <vector>

class GitIgnore
{
    private:
        std::vector<std::string> patterns;
        std::filesystem::path repo_path;
    public:
        GitIgnore(const std::filesystem::path& repo_root);
        bool load();
        bool isIgnored(const std::filesystem::path& path) const;
        static bool isOwnExecutable(const std::filesystem::path& path);
};