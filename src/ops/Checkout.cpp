#include "Checkout.h"
#include "../objects/ParsedObject.h"
#include "../infra/FileSystemUtils.h"
#include <stdexcept>

void restoreTree(const std::string& tree_id, const std::filesystem::path& target_dir, ObjectStore& store)
{
    std::string raw = store.readObject(tree_id);
    if (raw.empty())
    {
        throw std::runtime_error("Error: failed to read tree object: " + tree_id);
    }

    ParsedObject parse = ParsedObject::parse(raw);
    std::vector<TreeEntry> entries = parse.parseTreePayload();
    for (const auto& entry : entries)
    {
        if (entry.getMode() == "100644")
        {
            std::string entry_raw = store.readObject(entry.getObjectId());
            if (entry_raw.empty())
            {
                throw std::runtime_error("Error: failed to read blob: " + entry.getObjectId());
            }
            ParsedObject entry_parse = ParsedObject::parse(entry_raw);
            std::filesystem::path file_path = target_dir / entry.getName();
            if (!FileSystemUtils::writeBinaryFile(file_path, entry_parse.payload()))
            {
                throw std::runtime_error("Error: failed to write file: " + file_path.string());
            }
        }
        else if (entry.getMode() == "40000")
        {
            std::filesystem::path dir_path = target_dir / entry.getName();
            if (!FileSystemUtils::createDirectories(dir_path))
            {
                throw std::runtime_error("Error: failed to create directory: " + dir_path.string());
            }
            restoreTree(entry.getObjectId(), dir_path, store);
        }
    }
}

std::vector<std::string> getTreeFiles(const std::string& tree_id, const std::string& prefix, ObjectStore& store)
{
    std::vector<std::string> result;

    std::string raw = store.readObject(tree_id);
    if (raw.empty())
    {
        throw std::runtime_error("Error: failed to read tree object: " + tree_id);
    }

    ParsedObject parsed = ParsedObject::parse(raw);
    std::vector<TreeEntry> entries = parsed.parseTreePayload();

    for (const TreeEntry& entry : entries)
    {
        if (entry.getMode() == "100644")
        {
            result.push_back(prefix + entry.getName());
        }
        else if (entry.getMode() == "40000")
        {
            std::vector<std::string> nested = getTreeFiles(entry.getObjectId(), prefix + entry.getName() + '/', store);
            result.insert(result.end(), nested.begin(), nested.end());
        }
    }

    return result;
}