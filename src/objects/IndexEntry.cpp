#include "IndexEntry.h"

IndexEntry::IndexEntry(const std::string& mode, const std::string& hash, const std::string& path)
    : mode(mode), hash(hash), path(path) {}


const std::string& IndexEntry::getMode() const
{
    return this->mode;
}

const std::string& IndexEntry::getHash() const
{
    return this->hash;
}

const std::string& IndexEntry::getPath() const
{
    return this->path;
}