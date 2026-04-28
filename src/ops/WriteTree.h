#pragma once
#include "../core/ObjectStore.h"
#include <filesystem>

std::string buildTree(const std::filesystem::path& dir, ObjectStore& store);