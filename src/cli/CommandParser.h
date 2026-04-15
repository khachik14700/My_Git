#pragma once
#include "CommandType.h"
#include <string>

struct ParsedCommand
{
    CommandType command_type;
    std::string path;
    bool valid;
    std::string error_msg;
};

class CommandParser
{
    public:
        static ParsedCommand parse(int argc, char **argv);
};