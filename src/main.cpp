#include "cli/CommandParser.h"
#include "core/Repository.h"
#include "objects/Blob.h"
#include "infra/Hash.h"
#include "infra/FileSystemUtils.h"
#include "core/ObjectStore.h"
#include "core/RepositoryPaths.h"
#include <iostream>
#include <filesystem>

int handleInit(const ParsedCommand& parsed, const std::filesystem::path& current_path)
{
    if (parsed.path.empty())
    {
        if (Repository::init(current_path))
        {
            std::cout << "Repository initialized successfully" << std::endl;
            return 0;
        }
        else
        {
            std::cerr << "Failed to initialize repository" << std::endl;
            return 1;
        }
    }
    else
    {
        if (Repository::init(parsed.path))
        {
            std::cout << "Repository initialized successfully" << std::endl;
            return 0;
        }
        else
        {
            std::cerr << "Failed to initialize repository" << std::endl;
            return 1;
        }
    }
}

int handleHashObject(const ParsedCommand& parsed, const std::filesystem::path& current_path)
{
    if (!FileSystemUtils::exists(parsed.path))
    {
        std::cerr << "File does not exist" << std::endl;
        return 1;
    }
    if (!FileSystemUtils::isRegularFile(parsed.path))
    {
        std::cerr << "Path is not a regular file" << std::endl;
        return 1;
    }
    
    std::string file_content;
    if (!FileSystemUtils::readBinaryFile(parsed.path, file_content))
    {
        std::cerr << "Failed to read file" << std::endl;
        return 1;
    }
    
    Blob blob(file_content);
    std::string raw_object = blob.serialize();
    std::string object_id = Hash::sha256(raw_object);
    if (!parsed.write)
    {
        std::cout << object_id << std::endl;
        return 0;
    }
    else
    {
        if (!Repository::isValid(current_path))
        {
            std::cerr << "Current directory is not a valid repository" << std::endl;
            return 1;
        }
        RepositoryPaths rep_path(current_path);
        std::filesystem::path obj_path = rep_path.objectsDir();
        ObjectStore object_store(obj_path);
        std::string stored_object_id = object_store.writeObject(raw_object);
        if (stored_object_id.empty())
        {
            std::cerr << "Failed to write object" << std::endl;
            return 1;
        }
        if (stored_object_id != object_id)
        {
            std::cerr << "Stored object id does not match computed object id" << std::endl;
            return 1;
        }
        std::cout << stored_object_id << std::endl;
        return 0;
    }
}

int main(int argc, char **argv)
{
    ParsedCommand parsed = CommandParser::parse(argc, argv);
    std::filesystem::path current_path = std::filesystem::current_path();
    if (!parsed.valid)
    {
        std::cerr << parsed.error_msg << std::endl;
        return 1;
    }
    if (parsed.command_type == CommandType::Init)
    {
        return handleInit(parsed, current_path);
    }
    else if (parsed.command_type == CommandType::HashObject)
    {
        return handleHashObject(parsed, current_path);
    }

    return 0;
}