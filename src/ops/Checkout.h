#pragma once
#include "../core/ObjectStore.h"
#include <filesystem>
#include <string>
#include <vector>

void restoreTree(const std::string& tree_id, const std::filesystem::path& target_dir, ObjectStore& store);
std::vector<std::string> getTreeFiles(const std::string& tree_id, const std::string& prefix, ObjectStore& store);