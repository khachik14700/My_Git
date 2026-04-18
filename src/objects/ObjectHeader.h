#pragma once
#include <string>
#include <cstddef>
#include "ObjectType.h"

std::string buildObjectHeader(ObjectType type, std::size_t payload_size);