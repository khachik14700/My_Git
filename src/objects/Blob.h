#pragma once
#include <string>
#include "ObjectType.h"

class Blob
{
    private:
        std::string data;
    
    public:
        Blob(const std::string& data);
        ObjectType type() const;
        std::string payload() const;
        std::string serialize() const;
};