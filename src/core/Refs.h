#pragma once
#include <filesystem>
#include <string>

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
};