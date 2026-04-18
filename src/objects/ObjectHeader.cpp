#include "ObjectHeader.h"

std::string buildObjectHeader(ObjectType type, std::size_t payload_size)
{
    std::string result = "";
    result += objectTypeToString(type);
    result += ' ';
    result += std::to_string(payload_size);
    result += '\0';
    return result;
}