#include "WriteTree.h"
#include "../objects/Tree.h"
#include "../infra/FileSystemUtils.h"
#include "../objects/Blob.h"
#include <set>

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

std::string buildTreeFromIndex(const std::vector<IndexEntry>& entries, const std::string& prefix, ObjectStore& store)
{
    Tree tree;
    std::set<std::string> processed_dirs;

    for (const auto& entry : entries)
    {
        std::string path = entry.getPath();

        if (path.find(prefix) != 0)
        {
            continue;
        }

        std::string relative = path.substr(prefix.size());

        std::size_t slash_pos = relative.find('/');
        if (slash_pos == std::string::npos) // file in this directory
        {
            tree.addEntry(TreeEntry("100644", relative, entry.getHash()));
        }
        else
        {
            std::string dir_name = relative.substr(0, slash_pos);
            if (processed_dirs.count(dir_name) != 0)
            {
                continue;
            }
            processed_dirs.insert(dir_name);

            std::string sub_id = buildTreeFromIndex(entries, prefix + dir_name + "/", store);
            tree.addEntry(TreeEntry("40000", dir_name, sub_id));
        }
    }
    std::string raw = tree.serialize();
    return store.writeObject(raw);
}