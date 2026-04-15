#include "CommandParser.h"

ParsedCommand CommandParser::parse(int argc, char **argv)
{
    ParsedCommand result;
    if (argc < 2) //not write command (mygit)
    {
        result.command_type = CommandType::Unknown;
        result.path = "";
        result.valid = false;
        result.error_msg = "No command provided";
        return result;
    }
    std::string command = argv[1];
    if (command == "init") // mygit init ...
    {
        if (argc >= 3) // mygit init path
        {
            result.command_type = CommandType::Init;
            result.path = argv[2];
            result.valid = true;
            result.error_msg = "";
            return result;
        }
        else
        {
            result.command_type = CommandType::Init;
            result.path = "";
            result.valid = true;
            result.error_msg = "";
            return result;
        }
    }
    else
    {
        result.command_type = CommandType::Unknown;
        result.path = "";
        result.valid = false;
        result.error_msg = "Unknown command";
        return result;
    }
}