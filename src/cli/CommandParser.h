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
    std::string config_key;
    std::string config_value;
};

class CommandParser
{
    public:
        static ParsedCommand parse(int argc, char **argv);
    
    private:
        static ParsedCommand parseInit(int argc, char **argv);
        static ParsedCommand parseHashObject(int argc, char **argv);
        static ParsedCommand parseCatFile(int argc, char **argv);
        static ParsedCommand parseWriteTree(int argc, char **argv);
        static ParsedCommand parseConfig(int argc, char** argv);
        static ParsedCommand parseAdd(int argc, char** argv);
        static ParsedCommand makeError(const std::string& msg);
};