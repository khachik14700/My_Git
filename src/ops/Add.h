#pragma once
#include "../core/ObjectStore.h"
#include "../objects/Index.h"
#include <string>
#include <filesystem>

void addPath(const std::string& path, const std::filesystem::path& repo_root, ObjectStore& store, Index& index);