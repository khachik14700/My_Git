#include "ObjectType.h"

std::string objectTypeToString(ObjectType obj)
{
    switch (obj)
    {
    case ObjectType::Blob: return "blob";
    case ObjectType::Tree: return "tree";
    case ObjectType::Commit: return "commit";
    case ObjectType::Unknown: return "unknown";
    }
    return "unknown";
}

ObjectType stringToObjectType(const std::string& str)
{
    if (str == "blob") return ObjectType::Blob;
    if (str == "tree") return ObjectType::Tree;
    if (str == "commit") return ObjectType::Commit;

    return ObjectType::Unknown;
}