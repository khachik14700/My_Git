#include "Add.h"
#include "../infra/FileSystemUtils.h"
#include "../objects/Blob.h"
#include "../infra/GitIgnore.h"
#include "../objects/ParsedObject.h"
#include "../objects/TreeEntry.h"

static void addSingleFile(const std::filesystem::path& path, const std::filesystem::path& repo_root, ObjectStore& store, Index& index)
{
    std::string content = "";
    if (!FileSystemUtils::readBinaryFile(path, content))
    {
        throw std::runtime_error("Failed to read file: " + path.string());
    }
    Blob blob(content);
    std::string object_id = store.writeObject(blob.serialize());
    std::string relative_path = std::filesystem::relative(path, repo_root).string();
    index.add(IndexEntry("100644", object_id, relative_path));
}

void addPath(const std::string& path, const std::filesystem::path& repo_root, ObjectStore& store, Index& index)
{
    GitIgnore gitignore(repo_root);
    gitignore.load();

    if (path == ".")
    {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(repo_root))
        {
            std::filesystem::path entry_path = entry.path();
            std::filesystem::path git_dir = repo_root / ".git";
            if (entry_path.string().find(git_dir.string()) == 0)
            {
                continue;
            }
            if (FileSystemUtils::isRegularFile(entry_path))
            {
                if (GitIgnore::isOwnExecutable(entry_path))
                {
                    continue;
                }
                if (gitignore.isIgnored(entry_path))
                {
                    continue;
                }
                addSingleFile(entry_path, repo_root, store, index);
            }
        }
    }
    else
    {
        std::filesystem::path full_path = repo_root / path;
        if (!FileSystemUtils::exists(full_path))
        {
            throw std::runtime_error("path not found: " + path);
        }
        if (!FileSystemUtils::isRegularFile(full_path))
        {
            throw std::runtime_error("not a regular file: " + path);
        }
        if (GitIgnore::isOwnExecutable(full_path))
        {
            throw std::runtime_error("cannot add own executable");
        }
        if (gitignore.isIgnored(full_path))
        {
            throw std::runtime_error("file is ignored: " + path);
        }
        addSingleFile(full_path, repo_root, store, index);
    }
}

void buildIndexFromTree(const std::string& tree_id, const std::string& prefix, ObjectStore& store, Index& index)
{
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
            std::string path = prefix + entry.getName();
            index.add(IndexEntry(entry.getMode(), entry.getObjectId(), path));
        }
        else if (entry.getMode() == "40000")
        {
            buildIndexFromTree(entry.getObjectId(), prefix + entry.getName() + '/', store, index);
        }
    }
}