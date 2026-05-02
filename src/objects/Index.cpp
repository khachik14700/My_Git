#include "Index.h"
#include "../infra/FileSystemUtils.h"
#include <algorithm>


const std::vector<IndexEntry>& Index::getEntries() const
{
    return this->entries;
}

void Index::add(const IndexEntry& entry)
{
    for (auto& exists_entry : entries)
    {
        if (entry.getPath() == exists_entry.getPath())
        {
            exists_entry = entry;
            return;
        }
    }
    entries.push_back(entry);
}

bool Index::save(const std::filesystem::path& path) const
{
    std::string result = "";
    for (const auto& entry : entries)
    {
        result += entry.getMode() + " " + entry.getHash() + " " + entry.getPath() + "\n";
    }

    return FileSystemUtils::writeTextFile(path, result);
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

bool Index::load(const std::filesystem::path& path)
{
    entries.clear();
    if (!FileSystemUtils::exists(path))
    {
        return true;
    }
    std::string content = "";
    if (!FileSystemUtils::readBinaryFile(path, content))
    {
        return false;
    }
    size_t pos = 0;
    while (pos < content.size())
    {
        size_t new_line_pos = content.find('\n', pos);
        if (new_line_pos == std::string::npos)
        {
            new_line_pos = content.size();
        }

        std::string line = content.substr(pos, new_line_pos - pos);
        pos = new_line_pos + 1;

        line = trim(line);

        if (line.empty()) continue;

        size_t space1 = line.find(' ');
        if (space1 == std::string::npos)
            continue;

        size_t space2 = line.find(' ', space1 + 1);
        if (space2 == std::string::npos)
            continue;
        
        std::string mode = trim(line.substr(0, space1));
        std::string hash = trim(line.substr(space1 + 1, space2 - space1 - 1));
        std::string path = trim(line.substr(space2 + 1));

        if (!mode.empty() && !hash.empty() && !path.empty())
        {
            entries.push_back(IndexEntry(mode, hash, path));
        }
    }
    return true;
}

void Index::remove(const std::string& path)
{
    entries.erase(
                std::remove_if(entries.begin(), entries.end(),
                [&path](const IndexEntry& entry){return entry.getPath() == path;}),
                entries.end()
            );                
}