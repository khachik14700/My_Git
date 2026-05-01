#pragma once
#include <string>
#include <filesystem>
#include <map>

class RepositoryConfig
{
    private:
        std::map<std::string, std::map<std::string, std::string>> data_;
    public:
        RepositoryConfig();
        bool load(const std::filesystem::path& path);
        bool save(const std::filesystem::path& path) const;
        std::string get(const std::string& section, const std::string& key) const;
        void set(const std::string& section, const std::string& key, const std::string& value);
        std::string serialize() const;
};