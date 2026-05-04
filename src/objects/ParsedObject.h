#pragma once
#include "TreeEntry.h"
#include <string>
#include <vector>

class ParsedObject
{
    private:
        std::string type_;
        std::size_t size_;
        std::string payload_;

    public:
        ParsedObject(const std::string& type, std::size_t size, const std::string& payload);
        const std::string& type() const;
        std::size_t size() const;
        const std::string& payload() const;
        static ParsedObject parse(const std::string& raw_object);
        std::vector<TreeEntry> parseTreePayload() const;
        std::string parseCommitTreeId() const;
};