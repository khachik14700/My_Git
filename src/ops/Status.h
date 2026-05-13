#pragma once
#include "../core/ObjectStore.h"
#include "../objects/Index.h"
#include <vector>
#include <string>
#include <filesystem>

struct StatusResult
{
    // index vs HEAD
    std::vector<std::string> staged_new;
    std::vector<std::string> staged_modified;
    std::vector<std::string> staged_deleted;

    // working tree vs index
    std::vector<std::string> unstaged_modified;
    std::vector<std::string> unstaged_deleted;

    // untracked
    std::vector<std::string> untracked;
};

StatusResult getStatus(const std::filesystem::path& repo_root, ObjectStore& store, const Index& index, const std::string& head_commit_id);