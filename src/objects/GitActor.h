#pragma once
#include <string>
#include <cstdint>

struct GitActor
{
    std::string name;
    std::string email;
    std::int64_t timestamp;
    std::string timezone;
};