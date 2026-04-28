#include "WriteTree.h"
#include "../objects/Tree.h"
#include "../infra/FileSystemUtils.h"
#include "../objects/Blob.h"

std::string buildTree(const std::filesystem::path& dir, ObjectStore& store)
{
    Tree tree;
    for (const auto& entry : std::filesystem::directory_iterator(dir))
    {
        std::filesystem::path path = entry.path();
        std::string name = path.filename().string();
        if (name == ".git")
        {
            continue;
        }
        if (FileSystemUtils::isRegularFile(path))
        {
            std::string content;
            if (!FileSystemUtils::readBinaryFile(path, content))
            {
                throw std::runtime_error("Failed to read file: " + path.string());
            }
            Blob blob(content);
            std::string object_id = store.writeObject(blob.serialize());
            if (object_id.empty())
            {
                throw std::runtime_error("Failed to write blob object for file: " + path.string());
            }
            tree.addEntry(TreeEntry("100644", name, object_id));
        }
        else if (FileSystemUtils::isDirectory(path))
        {
            std::string object_id = buildTree(path, store);
            if (object_id.empty())
            {
                throw std::runtime_error("Failed to build tree for directory: " + path.string());
            }
            tree.addEntry(TreeEntry("40000", name, object_id));
        }
    }
    std::string raw = tree.serialize();
    std::string tree_id = store.writeObject(raw);
    if (tree_id.empty())
    {
        throw std::runtime_error("Failed to write tree object for directory: " + dir.string());
    }
    return tree_id;
}