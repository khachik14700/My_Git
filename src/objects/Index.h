#pragma once
#include "IndexEntry.h"
#include <vector>
#include <filesystem>

class Index
{
    private:
        std::vector<IndexEntry> entries;
    public:
        bool load(const std::filesystem::path& path);
        bool save(const std::filesystem::path& path) const;
        void add(const IndexEntry& entry);
        const std::vector<IndexEntry>& getEntries() const;
};