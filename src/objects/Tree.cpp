#include "Tree.h"
#include "../infra/Hash.h"
#include "ObjectHeader.h"
#include <algorithm>

ObjectType Tree::type() const
{
    return ObjectType::Tree;
}

std::string Tree::payload() const
{
        std::vector<TreeEntry> sorted = entries;

        std::sort(sorted.begin(), sorted.end(), [](const TreeEntry& a, const TreeEntry& b){
                return a.getName() < b.getName();                
        });

        std::string result;
        for (const TreeEntry& entry : sorted)
        {
                result += entry.getMode() + " " + entry.getName();
                result += '\0';
                result += Hash::hexToBytes(entry.getObjectId());    
        }
        return result;
}

std::string Tree::serialize() const
{
        std::string payload = this->payload();
        std::string header = buildObjectHeader(this->type(), payload.size());
        return header + payload;
}

void Tree::addEntry(const TreeEntry& entry)
{
        this->entries.emplace_back(entry);
}

const std::vector<TreeEntry>& Tree::getEntries() const
{
        return this->entries;
}