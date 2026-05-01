#include "RepositoryConfig.h"
#include "../infra/FileSystemUtils.h"

RepositoryConfig::RepositoryConfig()
{
    data_["core"]["repositoryformatversion"] = "0";
    data_["core"]["filemode"] = "true";
    data_["core"]["bare"] = "false";
}

void RepositoryConfig::set(const std::string& section, const std::string& key, const std::string& value)
{
    data_[section][key] = value;
}

std::string RepositoryConfig::get(const std::string& section, const std::string& key) const
{
    auto section_it = data_.find(section);
    if (section_it == data_.end())
    {
        return "";
    }
    
    auto key_it = section_it->second.find(key);
    if (key_it == section_it->second.end())
    {
        return "";
    }
    return key_it->second;
}

std::string RepositoryConfig::serialize() const
{
    std::string result = "";
    for (const auto& section : data_)
    {
        result += "[" + section.first + "]\n";

        for (const auto& key : section.second)
        {
            result += "\t" + key.first + " = " + key.second + "\n";
        }
    }
    return result;
}

bool RepositoryConfig::save(const std::filesystem::path& path) const
{
    std::string content = serialize();
    return FileSystemUtils::writeTextFile(path, content);
}

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

bool RepositoryConfig::load(const std::filesystem::path& path)
{
    data_.clear();
    std::string content = "";
    if (!FileSystemUtils::readBinaryFile(path, content))
    {
        return false;
    }
    std::size_t pos = 0;
    std::string current_section = "";
    while (pos < content.size())
    {
        std::size_t new_line_pos = content.find('\n', pos);
        if (new_line_pos == std::string::npos)
        {
            new_line_pos = content.size();
        }
        std::string line = content.substr(pos, new_line_pos - pos);
        pos = new_line_pos + 1;

        std::size_t start = line.find_first_not_of(" \t");
        if (start == std::string::npos) continue;
        line = line.substr(start);

        size_t end = line.find_last_not_of(" \t");
        line = line.substr(0, end + 1);

        if (line[0] == ';' || line[0] == '#')
            continue;

        if (line[0] == '[')
        {
            std::size_t close_bracket = line.find(']', 1);
            if (close_bracket != std::string::npos)
            {
                current_section = line.substr(1, close_bracket - 1);
            }
            continue;
        }
        std::size_t equal_pos = line.find('=');
        if (equal_pos == std::string::npos) continue;

        std::string key = line.substr(0, equal_pos);
        std::string value = line.substr(equal_pos + 1);

        key = trim(key);
        value = trim(value);
        
        if (!key.empty())
        {
            data_[current_section][key] = std::move(value);
        }
    }

    return true;
}