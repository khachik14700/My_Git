#pragma once
#include "TreeEntry.h"
#include "ObjectType.h"
#include <vector>
#include <string>

class Tree
{
    private:
        std::vector<TreeEntry> entries;
    
    public:
        void addEntry(const TreeEntry& entry);
        const std::vector<TreeEntry>& getEntries() const;
        ObjectType type() const;
        std::string payload() const;
        std::string serialize() const;
};