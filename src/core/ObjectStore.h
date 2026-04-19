#pragma once
#include <string>
#include <filesystem>

class ObjectStore
{
    private:
        std::filesystem::path objects_dir;
    
    public:
        ObjectStore(const std::filesystem::path& objects_dir);
        std::string writeObject(const std::string& raw_object);
        std::string readObject(const std::string& object_id) const;
};