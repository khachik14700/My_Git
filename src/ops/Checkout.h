#pragma once
#include "../core/ObjectStore.h"
#include <filesystem>
#include <string>

void restoreTree(const std::string& tree_id, const std::filesystem::path& target_dir, ObjectStore& store);