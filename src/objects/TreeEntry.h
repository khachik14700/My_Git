#pragma once
#include <string>

class TreeEntry
{
    private:
        std::string mode;
        std::string name;
        std::string object_id;
    
    public:
        TreeEntry (const std::string& mode, const std::string& name, const std::string& object_id);
        const std::string& getMode() const;
        const std::string& getName() const;
        const std::string& getObjectId() const;
};