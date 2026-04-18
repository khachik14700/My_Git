#pragma once
#include <string>

enum class ObjectType
{
    Blob,
    Tree,
    Commit,
    Unknown
};

std::string objectTypeToString(ObjectType obj);
ObjectType stringToObjectType(const std::string& str);