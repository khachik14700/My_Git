#include "Status.h"
#include "../objects/ParsedObject.h"
#include "../objects/TreeEntry.h"
#include "../infra/FileSystemUtils.h"
#include "../objects/Blob.h"
#include "../infra/Hash.h"
#include "../infra/GitIgnore.h"
#include <map>


static std::map<std::string, std::string> getTreeFilesMap(const std::string& tree_id, const std::string& prefix, ObjectStore& store)
{
    std::map<std::string, std::string> result;
    std::string raw = store.readObject(tree_id);
    if (raw.empty())
    {
        throw std::runtime_error("Error: failed to read tree object: " + tree_id);
    }
    
    ParsedObject payload = ParsedObject::parse(raw);
    std::vector<TreeEntry> entries = payload.parseTreePayload();

    for (const TreeEntry& entry : entries)
    {
        if (entry.getMode() == "100644")
        {
            result[prefix + entry.getName()] = entry.getObjectId();
        }
        else if (entry.getMode() == "40000")
        {
            auto nested = getTreeFilesMap(entry.getObjectId(), prefix + entry.getName() + '/', store);
            result.insert(nested.begin(), nested.end());
        }
    }
    return result;
}

StatusResult getStatus(const std::filesystem::path& repo_root, ObjectStore& store, const Index& index, const std::string& head_commit_id)
{
    StatusResult result;
    std::map<std::string, std::string> head_files = {};
    std::map<std::string, std::string> index_files = {};

    if (!head_commit_id.empty())
    {
        std::string raw = store.readObject(head_commit_id);
        ParsedObject parsed = ParsedObject::parse(raw);
        std::string tree_id = parsed.parseCommitTreeId();

        head_files = getTreeFilesMap(tree_id, "", store);
    }

    for (const IndexEntry& entry : index.getEntries())
    {
        index_files[entry.getPath()] = entry.getHash();
    }

    // index vs HEAD
    for (const std::pair<const std::string, std::string>& index_file : index_files)
    {
        if (head_files.find(index_file.first) == head_files.end())
        {
            result.staged_new.push_back(index_file.first);
        }
        else if (index_file.second != head_files[index_file.first])
        {
            result.staged_modified.push_back(index_file.first);
        }
    }
    for (const auto& head_file : head_files)
    {
        if (index_files.find(head_file.first) == index_files.end())
        {
            result.staged_deleted.push_back(head_file.first);
        }
    }

    // working tree vs index
    for (const auto& index_file : index_files)
    {
        std::filesystem::path full_path = repo_root / index_file.first;
        if (!FileSystemUtils::exists(full_path))
        {
            result.unstaged_deleted.push_back(index_file.first);
        }
        else
        {
            std::string content;
            FileSystemUtils::readBinaryFile(full_path, content);
            Blob blob(content);
            std::string hash = Hash::sha256(blob.serialize());
            if (hash != index_file.second)
            {
                result.unstaged_modified.push_back(index_file.first);
            }
        } 
    }

    // untracked files
    GitIgnore ignore(repo_root);
    ignore.load();
    for (const auto& file : std::filesystem::recursive_directory_iterator(repo_root))
    {
        std::filesystem::path file_path = file.path();
        std::filesystem::path git_directory = repo_root / ".git";
        if (file_path.string().find(git_directory.string()) == 0)
        {
            continue;
        }

        if (FileSystemUtils::isRegularFile(file_path))
        {
            if (GitIgnore::isOwnExecutable(file_path))
            {
                continue;
            }
            if (ignore.isIgnored(file_path))
            {
                continue;
            }
            std::string relative = std::filesystem::relative(file_path, repo_root).string();
            if (index_files.find(relative) == index_files.end())
            {
                result.untracked.push_back(relative);
            }
        }
    }

    return result;
}