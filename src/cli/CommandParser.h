#pragma once
#include "CommandType.h"
#include <string>

enum class CatFileMode
{
    None,
    Type,
    Size,
    Exists,
    PrettyPrint
};

struct ParsedCommand
{
    CommandType command_type;
    std::string path;
    bool write;
    bool valid;
    CatFileMode cat_file_mode;
    std::string error_msg;
};

class CommandParser
{
    public:
        static ParsedCommand parse(int argc, char **argv);
};