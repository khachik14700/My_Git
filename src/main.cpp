#include "cli/CommandParser.h"
#include "core/Repository.h"
#include "objects/Blob.h"
#include "infra/Hash.h"
#include "infra/FileSystemUtils.h"
#include "core/ObjectStore.h"
#include "core/RepositoryPaths.h"
#include "objects/ParsedObject.h"
#include "ops/WriteTree.h"
#include "core/RepositoryConfig.h"
#include "ops/Add.h"
#include <iostream>
#include <filesystem>
#include <set>

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
            std::cerr << "Error: Failed to initialize repository " << std::endl;
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
            std::cerr << "Error: Failed to initialize repository " << std::endl;
            return 1;
        }
    }
}

int handleHashObject(const ParsedCommand& parsed, const std::filesystem::path& current_path)
{
    if (!FileSystemUtils::exists(parsed.path))
    {
        std::cerr << "Error: File does not exist " << parsed.path << std::endl;
        return 1;
    }
    if (!FileSystemUtils::isRegularFile(parsed.path))
    {
        std::cerr << "Error: Path is not a regular file " << parsed.path << std::endl;
        return 1;
    }
    
    std::string file_content;
    if (!FileSystemUtils::readBinaryFile(parsed.path, file_content))
    {
        std::cerr << "Error: Failed to read file " << parsed.path << std::endl;
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
            std::cerr << "Error: Current directory is not a valid repository" << std::endl;
            return 1;
        }
        RepositoryPaths rep_path(current_path);
        std::filesystem::path obj_path = rep_path.objectsDir();
        ObjectStore object_store(obj_path);
        std::string stored_object_id = object_store.writeObject(raw_object);
        if (stored_object_id.empty())
        {
            std::cerr << "Error: Failed to write object" << std::endl;
            return 1;
        }
        if (stored_object_id != object_id)
        {
            std::cerr << "Error: Stored object id does not match computed object id" << std::endl;
            return 1;
        }
        std::cout << stored_object_id << std::endl;
        return 0;
    }
}

int handleCatFile(const ParsedCommand& parsed, const std::filesystem::path& current_path)
{
    if (!Repository::isValid(current_path))
    {
        std::cerr << "Error: Current directory is not a valid repository" << std::endl;
        return 1;
    }
    RepositoryPaths repository_paths(current_path);
    std::filesystem::path objects_dir = repository_paths.objectsDir();
    ObjectStore object_store(objects_dir);
    std::string raw_object = object_store.readObject(parsed.path);
    if (raw_object.empty())
    {
        std::cerr << "Error: Object not found: " << parsed.path << std::endl;
        return 1;
    }

    try
    {
        ParsedObject parsed_object = ParsedObject::parse(raw_object);
        
        if (parsed.cat_file_mode == CatFileMode::Type)
        {
            std::cout << parsed_object.type() << std::endl;
            return 0;
        }
        else if (parsed.cat_file_mode == CatFileMode::Size)
        {
            std::cout << parsed_object.size() << std::endl;
            return 0;
        }
        else if (parsed.cat_file_mode == CatFileMode::Exists)
        {
            return 0;
        }
        else if (parsed.cat_file_mode == CatFileMode::PrettyPrint)
        {
            if (parsed_object.type() == "blob")
            {
                std::cout << parsed_object.payload() << std::endl;
                return 0;
            }
            else if (parsed_object.type() == "tree")
            {
                std::vector<TreeEntry> entries = parsed_object.parseTreePayload();
                for (const auto& entry : entries)
                {
                    std::string display_mode = (entry.getMode() == "40000") ? "040000" : entry.getMode();
                    std::string obj_type = (entry.getMode() == "40000") ? "tree" : "blob";
                    std::cout << display_mode << " " << obj_type << " " << entry.getObjectId() << "    " << entry.getName() << std::endl;
                }
                return 0;
            }
            else
            {
                std::cerr << "Error: Pretty-print not supported for this object type" << std::endl;
                return 1;
            }
        }
        else
        {
            std::cerr << "Error: Unsupported cat-file mode" << std::endl;
            return 1;
        }
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

int handleWriteTree(const ParsedCommand& parsed, const std::filesystem::path& current_path)
{
    if (!Repository::isValid(current_path))
    {
        std::cerr << "Error: Current directory is not a valid repository" << std::endl;
        return 1;
    }
    RepositoryPaths repository_paths(current_path);
    std::filesystem::path objects_dir = repository_paths.objectsDir();
    ObjectStore store(objects_dir);
    try
    {
        if (parsed.from_fs)
        {
            std::string tree_id = buildTree(current_path, store);
            if (tree_id.empty())
            {
                throw std::runtime_error("Error: failed to write tree object");
            }
            std::cout << tree_id << std::endl;
        }
        else
        {
            Index index;
            std::filesystem::path index_path = repository_paths.indexFile();
            if (!index.load(index_path))
            {
                throw std::runtime_error("Error: failed to load index");
            }
            std::string tree_id = buildTreeFromIndex(index.getEntries(), "", store);
            if (tree_id.empty())
            {
                throw std::runtime_error("Error: failed to write tree object from index");
            }
            std::cout << tree_id << std::endl;
        }
    }
    catch (const std::runtime_error& er)
    {
        std::cerr << "Error: " <<  er.what() << std::endl;
        return 1;
    }

    return 0;
}

int handleConfig(const ParsedCommand& parsed, const std::filesystem::path& current_path)
{
    if (!Repository::isValid(current_path))
    {
        std::cerr << "Error: Current directory is not a valid repository" << std::endl;
        return 1;
    }

    RepositoryPaths repo_path(current_path);
    std::filesystem::path config_path = repo_path.configFile();
    RepositoryConfig config;

    if (!config.load(config_path))
    {
        std::cerr << "Error: failed to load config file" << std::endl;
        return 1;
    }

    std::size_t dot =  parsed.config_key.find('.');
    std::string section = parsed.config_key.substr(0, dot);
    std::string key = parsed.config_key.substr(dot + 1);

    static const std::set<std::string> allowed_sections = {"core", "user"};
    if (allowed_sections.count(section) == 0)
    {
        std::cerr << "Error: unknown section '" << section << "'" << std::endl;
        return 1;
    }

    if (parsed.config_value.empty())
    {
        std::string value = config.get(section, key);
        if (value.empty())
        {
            std::cerr << "Error: key '" << parsed.config_key << "' not found" << std::endl;
            return 1;
        }
        std::cout << value << std::endl;
        return 0;
    }
    else
    {
        config.set(section, key, parsed.config_value);
        if (!config.save(config_path))
        {
            std::cerr << "Error: failed to save config file" << std::endl;
            return 1;
        }
        return 0;
    }
}

int handleAdd(const ParsedCommand& parsed, const std::filesystem::path& current_path)
{
    if (!Repository::isValid(current_path))
    {
        std::cerr << "Error: Current directory is not a valid repository" << std::endl;
        return 1;
    }

    RepositoryPaths repo_paths(current_path);
    std::filesystem::path objs_path = repo_paths.objectsDir();
    std::filesystem::path index_path = repo_paths.indexFile();

    ObjectStore store(objs_path);
    Index index;

    if (!index.load(index_path))
    {
        std::cerr << "Error: failed to load index" << std::endl;
        return 1;
    }

    try
    {
        addPath(parsed.path, current_path, store, index);
        if (!index.save(index_path))
        {
            throw std::runtime_error("Error: failed to save index");
        }
    }
    catch (const std::runtime_error& er)
    {
        std::cerr << "Error: " << er.what() << std::endl;
        return 1;
    }

    return 0;
}

int handleRm(const ParsedCommand& parsed, const std::filesystem::path& current_path)
{
    if (!Repository::isValid(current_path))
    {
        std::cerr << "Error: Current directory is not a valid repository" << std::endl;
        return 1;
    }

    RepositoryPaths repo_paths(current_path);
    std::filesystem::path index_path = repo_paths.indexFile();

    Index index;
    if (!index.load(index_path))
    {
        std::cerr << "Error: failed to load index" << std::endl;
        return 1;
    }

    std::filesystem::path relative_path =
    std::filesystem::path(parsed.path).lexically_normal();

    std::filesystem::path full_path = current_path / relative_path;

    bool removed = index.remove(relative_path.string());

    if (!removed)
    {
        std::cerr << "Error: file is not in index" << std::endl;
        return 1;
    }

    if (!parsed.cached)
    {
        if (FileSystemUtils::exists(full_path))
        {
            if (!FileSystemUtils::isRegularFile(full_path))
            {
                std::cerr << "Error: not a regular file" << std::endl;
                return 1;
            }

            std::error_code ec;
            std::filesystem::remove(full_path, ec);

            if (ec)
            {
                std::cerr << "Error: failed to remove file" << std::endl;
                return 1;
            }
        }
    }

    if (!index.save(index_path))
    {
        std::cerr << "Error: failed to save index" << std::endl;
        return 1;
    }

    return 0;
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
    else if (parsed.command_type == CommandType::CatFile)
    {
        return handleCatFile(parsed, current_path);
    }
    else if (parsed.command_type == CommandType::WriteTree)
    {
        return handleWriteTree(parsed, current_path);
    }
    else if (parsed.command_type == CommandType::Config)
    {
        return handleConfig(parsed, current_path);
    }
    else if (parsed.command_type == CommandType::Add)
    {
        return handleAdd(parsed, current_path);
    }
    else if (parsed.command_type == CommandType::Rm)
    {
        return handleRm(parsed, current_path);
    }

    return 0;
}