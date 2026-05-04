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

enum class BranchMode
{
    None,
    List,
    Create,
    Delete,
    Rename
};

struct ParsedCommand
{
    CommandType command_type;
    std::string path;
    bool write;
    bool valid;
    bool from_fs;
    bool cached;
    bool create_branch;
    CatFileMode cat_file_mode;
    BranchMode branch_mode;
    std::string branch_new_name;
    std::string config_key;
    std::string config_value;
    std::string commit_message;
    std::string error_msg;
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
        static ParsedCommand parseRm(int argc, char** argv);
        static ParsedCommand parseCommit(int argc, char** argv);
        static ParsedCommand parseBranch(int argc, char** argv);
        static ParsedCommand parseSwitch(int argc, char** argv);
        static ParsedCommand makeError(const std::string& msg);
};