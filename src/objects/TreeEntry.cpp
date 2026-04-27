#include "TreeEntry.h"

TreeEntry::TreeEntry (const std::string& mode, const std::string& name, const std::string& object_id)
    : mode(mode), name(name), object_id(object_id) {}


const std::string& TreeEntry::getMode() const
{
    return this->mode;
}


const std::string& TreeEntry::getName() const
{
    return this->name;
}


const std::string& TreeEntry::getObjectId() const
{
    return this->object_id;
}