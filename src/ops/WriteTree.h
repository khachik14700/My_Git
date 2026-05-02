#pragma once
#include "../core/ObjectStore.h"
#include "../objects/IndexEntry.h"
#include <vector>
#include <filesystem>

std::string buildTree(const std::filesystem::path& dir, ObjectStore& store);
std::string buildTreeFromIndex(const std::vector<IndexEntry>& entries, const std::string& prefix, ObjectStore& store);