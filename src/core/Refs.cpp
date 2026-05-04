#include "Refs.h"
#include "../infra/FileSystemUtils.h"
#include "RepositoryPaths.h"

Refs::Refs(const std::filesystem::path& repo_root): repo_root(repo_root) {}

std::string Refs::readHead()
{
    RepositoryPaths repo_paths(repo_root);
    std::filesystem::path head_path = repo_paths.headFile();
    std::string content = "";
    if (!FileSystemUtils::exists(head_path))
    {
        throw std::runtime_error("Error: " + head_path.string() + " is not exists");
    }
    if (!FileSystemUtils::isRegularFile(head_path))
    {
        throw std::runtime_error("Error: " + head_path.string() + " is not a regular file");
    }
    if (!FileSystemUtils::readBinaryFile(head_path, content))
    {
        throw std::runtime_error("Error: failed to read HEAD file");
    }
    if (content.empty())
    {
        throw std::runtime_error("Error: HEAD file is empty");
    }
    if (content.back() == '\n')
    {
        content.pop_back();
    }
    std::string prefix = "ref: refs/heads/";
    if (content.find(prefix) == 0)
    {
        return content.substr(prefix.size());
    }
    else
    {
        return content;
    }
}

std::string Refs::readBranch(const std::string& branch_name)
{
    RepositoryPaths repo_paths(repo_root);
    std::filesystem::path branch_path = repo_paths.headsDir() / branch_name;
    if (!FileSystemUtils::exists(branch_path))
    {
        return "";
    }
    if (!FileSystemUtils::isRegularFile(branch_path))
    {
        throw std::runtime_error("Error: " + branch_path.string() + " is not a regular file");
    }
    std::string content = "";
    if (!FileSystemUtils::readBinaryFile(branch_path, content))
    {
        throw std::runtime_error("Error: failed to read Branch file");
    }
    if (content.empty())
    {
        throw std::runtime_error("Error: Branch file is empty");
    }
    if (content.back() == '\n')
    {
        content.pop_back();
    }
    return content;
}

bool Refs::updateBranch(const std::string& branch_name, const std::string& commit_id)
{
    RepositoryPaths repo_paths(repo_root);
    std::filesystem::path branch_path = repo_paths.headsDir() / branch_name;
    std::string content = commit_id;
    content += '\n';
    return FileSystemUtils::writeTextFile(branch_path, content);
}

bool Refs::updateHead(const std::string& branch_name)
{
    RepositoryPaths repo_paths(repo_root);
    std::filesystem::path head_path = repo_paths.headFile();
    std::string content = "ref: refs/heads/" + branch_name + '\n';
    if (!FileSystemUtils::exists(head_path))
    {
        throw std::runtime_error("Error: " + head_path.string() + " is not exists");
    }
    if (!FileSystemUtils::isRegularFile(head_path))
    {
        throw std::runtime_error("Error: " + head_path.string() + " is not a regular file");
    }
    return FileSystemUtils::writeTextFile(head_path, content);
}

std::vector<std::string> Refs::listBranches()
{
    std::vector<std::string> result;
    RepositoryPaths repo_paths(repo_root);
    std::filesystem::path heads_dir_path = repo_paths.headsDir();
    for (const auto& entry : std::filesystem::directory_iterator(heads_dir_path))
    {
        if (FileSystemUtils::isRegularFile(entry.path()))
        {
            std::string name = entry.path().filename().string();
            result.push_back(name);
        }
    }
    return result;
}

bool Refs::createBranch(const std::string& branch_name, const std::string& commit_id)
{
    RepositoryPaths repo_paths(repo_root);
    std::filesystem::path branch_path = repo_paths.headsDir() / branch_name;
    if (FileSystemUtils::exists(branch_path))
    {
        throw std::runtime_error("Error: branch '" + branch_name + "' already exists");
    }
    return FileSystemUtils::writeTextFile(branch_path, commit_id + "\n");
}

bool Refs::deleteBranch(const std::string& branch_name)
{
    RepositoryPaths repo_paths(repo_root);
    std::filesystem::path branch_path = repo_paths.headsDir() / branch_name;
    if (!FileSystemUtils::exists(branch_path))
    {
        throw std::runtime_error("Error: branch '" + branch_name + "' does not exist");
    }
    std::string current_branch = readHead();
    if (current_branch == branch_name)
    {
        throw std::runtime_error("Error: cannot delete current branch '" + branch_name + "'");
    }
    std::error_code ec;
    std::filesystem::remove(branch_path, ec);
    if (ec)
    {
        return false;
    }
    return true;
}

bool Refs::renameBranch(const std::string& old_name, const std::string& new_name)
{
    RepositoryPaths repo_paths(repo_root);
    std::filesystem::path branch_path = repo_paths.headsDir() / old_name;
    if (!FileSystemUtils::exists(branch_path))
    {
        throw std::runtime_error("Error: branch '" + old_name + "' does not exist");
    }

    std::filesystem::path new_branch_path = repo_paths.headsDir() / new_name;
    if (FileSystemUtils::exists(new_branch_path))
    {
        throw std::runtime_error("Error: branch '" + new_name + "' already exists");
    }

    std::error_code ec;
    std::filesystem::rename(branch_path, new_branch_path, ec);
    if (ec)
    {
        return false;
    }

    if (readHead() == old_name)
    {
        if (!updateHead(new_name))
        {
            return false;
        }
    }

    return true;
}