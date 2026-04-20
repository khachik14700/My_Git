#include "CommandParser.h"

ParsedCommand CommandParser::parse(int argc, char **argv)
{
    ParsedCommand result;
    result.command_type = CommandType::Unknown;
    result.path = "";
    result.valid = false;
    result.write = false;
    result.cat_file_mode = CatFileMode::None;
    result.error_msg = "";
    if (argc < 2) //not write command (mygit)
    {
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
            return result;
        }
        else
        {
            result.command_type = CommandType::Init;
            result.valid = true;
            return result;
        }
    }
    else if (command == "hash-object")
    {
        result.command_type = CommandType::HashObject;

        if (argc < 3)
        {
            result.valid = false;
            result.error_msg = "No file path provided";
            return result;
        }
        std::string first_arg = argv[2];
        if (first_arg == "-w") // mygit hash-object -w ...
        {
            result.write = true;
            if (argc >= 4)
            {
                result.path = argv[3];
                result.valid = true;
                return result;
            }
            else
            {
                result.valid = false;
                result.error_msg = "No file path provided";
                return result;
            }
        }
        else // mygit hash-object ...
        {
            result.path = argv[2];
            result.valid = true;
            return result;
        }
    }
    else if (command == "cat-file")
    {
        result.command_type = CommandType::CatFile;
        if (argc < 4)
        {
            result.valid = false;
            result.error_msg = "cat-file requires a mode and object id";
            return result;
        }
        std::string mode = argv[2];
        std::string object_id = argv[3];
        if (mode == "-t")
        {
            result.cat_file_mode = CatFileMode::Type;
        }
        else if (mode == "-s")
        {
            result.cat_file_mode = CatFileMode::Size;
        }
        else if (mode == "-e")
        {
            result.cat_file_mode = CatFileMode::Exists;
        }
        else if (mode == "-p")
        {
            result.cat_file_mode = CatFileMode::PrettyPrint;
        }
        else
        {
            result.valid = false;
            result.error_msg = "Unknown cat-file mode";
            return result;
        }

        result.path = object_id;
        result.valid = true;
        return result;
    }
    else
    {
        result.error_msg = "Unknown command";
        return result;
    }
}