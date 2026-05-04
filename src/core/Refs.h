#pragma once
#include <filesystem>
#include <string>
#include <vector>

class Refs
{
    private:
        std::filesystem::path repo_root;
    public:
        Refs(const std::filesystem::path& repo_root);
        std::string readHead();
        std::string readBranch(const std::string& branch_name);
        bool updateBranch(const std::string& branch_name, const std::string& commit_id);
        bool updateHead(const std::string& branch_name);
        std::vector<std::string> listBranches();
        bool createBranch(const std::string& branch_name, const std::string& commit_id);
        bool deleteBranch(const std::string& branch_name);
        bool renameBranch(const std::string& old_name, const std::string& new_name);
};