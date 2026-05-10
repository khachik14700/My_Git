#pragma once
#include "../core/ObjectStore.h"
#include "../objects/Index.h"
#include <string>
#include <filesystem>

void addPath(const std::string& path, const std::filesystem::path& repo_root, ObjectStore& store, Index& index);
void buildIndexFromTree(const std::string& tree_id, const std::string& prefix, ObjectStore& store, Index& index);