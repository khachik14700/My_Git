#include "ParsedObject.h"
#include "../infra/Hash.h"
#include "Tree.h"
#include <stdexcept>

ParsedObject::ParsedObject(const std::string& type, std::size_t size, const std::string& payload)
{
    this->type_ = type;
    this->size_ = size;
    this->payload_ = payload;
}

const std::string& ParsedObject::type() const
{
    return this->type_;
}

std::size_t ParsedObject::size() const
{
    return this->size_;
}
 
const std::string& ParsedObject::payload() const
{
    return this->payload_;
}

ParsedObject ParsedObject::parse(const std::string& raw_object)
{
    size_t pos = raw_object.find('\0');
    if (pos == std::string::npos)
    {
        throw std::runtime_error("Invalid object format");
    }
    std::string header = raw_object.substr(0, pos);
    std::string payload = raw_object.substr(pos + 1);
    std::size_t space_pos = header.find(' ');
    if (space_pos == std::string::npos)
    {
        throw std::runtime_error("Invalid object header");
    }

    std::string object_type = header.substr(0, space_pos);
    std::string object_size_string = header.substr(space_pos + 1);
    std::size_t expected_size;
    try
    {
        expected_size = std::stoull(object_size_string);
    }
    catch(...)
    {
        throw std::runtime_error("Invalid object size in header");
    }

    if (expected_size != payload.size())
    {
        throw std::runtime_error("Object payload size does not match header size");
    }
    return ParsedObject(object_type, expected_size, payload);
}

std::vector<TreeEntry> ParsedObject::parseTreePayload() const
{
    if (type_ != "tree")
    {
        throw std::runtime_error("Object is not a tree");
    }
    std::vector<TreeEntry> entries;
    size_t pos = 0;
    while (pos < payload_.size())
    {
        size_t space_pos = payload_.find(' ', pos);
        std::string mode = payload_.substr(pos, space_pos - pos);
        pos = space_pos + 1;
        size_t null_pos = payload_.find('\0', pos);
        std::string name = payload_.substr(pos, null_pos - pos);
        pos = null_pos + 1;
        std::string binary_hash = payload_.substr(pos, 32);
        pos += 32;
        std::string hex = Hash::bytesToHex(binary_hash);
        entries.push_back(TreeEntry(mode, name, hex));
    }
    return entries;
}

std::string ParsedObject::parseCommitTreeId() const
{
    std::size_t new_line_pos = payload_.find_first_of('\n');
    std::string line = payload_.substr(0, new_line_pos);
    std::string prefix = "tree ";
    if (line.find(prefix) != 0)
    {
        throw std::runtime_error("Error: invalid commit format — missing tree id");
    }
    
    return line.substr(prefix.length());
}