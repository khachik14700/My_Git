#pragma once
#include <string>

class IndexEntry
{
    private:
        std::string mode;
        std::string hash;
        std::string path;
    public:
        IndexEntry(const std::string& mode, const std::string& hash, const std::string& path);
        const std::string& getMode() const;
        const std::string& getHash() const;
        const std::string& getPath() const;
};