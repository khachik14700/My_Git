#include "CommandParser.h"

ParsedCommand CommandParser::makeError(const std::string& msg)
{
    ParsedCommand result;
    result.valid = false;
    result.command_type = CommandType::Unknown;
    result.write = false;
    result.cat_file_mode = CatFileMode::None;
    result.path = "";
    result.config_key = "";
    result.config_value = "";
    result.error_msg = msg;
    return result;
}

ParsedCommand CommandParser::parseInit(int argc, char** argv)
{
    ParsedCommand result;
    result.command_type = CommandType::Init;
    result.valid = true;
    result.write = false;
    result.path = "";
    result.cat_file_mode = CatFileMode::None;
    result.error_msg = "";
    result.config_key = "";
    result.config_value = "";

    if (argc > 3)
    {
        return makeError("init: too many arguments");
    }
    else if (argc == 3) // mygit init path
    {
            result.path = argv[2];
    }
    return result;
}


ParsedCommand CommandParser::parseHashObject(int argc, char** argv)
{
    ParsedCommand result;
    result.command_type = CommandType::HashObject;
    result.path = "";
    result.valid = true;
    result.write = false;
    result.cat_file_mode = CatFileMode::None;
    result.error_msg = "";
    result.config_key = "";
    result.config_value = "";


    if (argc == 2)
    {
        return makeError("hash-object: no file provided");
    }

    if (argv[2][0] == '-')
    {
        if (std::string(argv[2]) == "-w")
        {
            if (argc == 3)
            {
                return makeError("hash-object: no file provided after -w");
            }
            else if (argc > 4)
            {
                return makeError("hash-object: too many arguments");
            }
            else if (argc == 4)
            {
                result.write = true;
                result.path = argv[3];
                return result;
            }
            return makeError("hash-object: unexpected error");
        }
        else
        {
            return makeError("hash-object: unknown flag: " + std::string(argv[2]));
        }
    }
    else
    {
        if (argc > 3)
        {
            return makeError("hash-object: too many arguments");
        }
        result.path = argv[2];
        return result;
    }
}


ParsedCommand CommandParser::parseCatFile(int argc, char** argv)
{
    if (argc == 2)
    {
        return makeError("cat-file: missing mode and object id");
    }
    else if (argc == 3)
    {
        return makeError("cat-file: missing object id");
    }
    else if (argc > 4)
    {
        return makeError("cat-file: too many arguments");
    }

    ParsedCommand result;
    result.command_type = CommandType::CatFile;
    result.valid = true;
    result.write = false;
    result.cat_file_mode = CatFileMode::None;
    result.error_msg = "";
    result.config_key = "";
    result.config_value = "";


    std::string mode = std::string(argv[2]);

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
        return makeError("cat-file: unknown mode: " + mode);
    }

    std::string object_id = argv[3];
    if (object_id.empty())
    {
        return makeError("cat-file: object id cannot be empty");
    }

    result.path = object_id;
    return result;
}


ParsedCommand CommandParser::parseWriteTree(int argc, char** argv)
{
    (void)argv;
    if (argc > 2)
    {
        return makeError("write-tree: too many arguments");
    }

    ParsedCommand result;
    result.command_type = CommandType::WriteTree;
    result.valid = true;
    result.write = false;
    result.path = "";
    result.cat_file_mode = CatFileMode::None;
    result.error_msg = "";
    result.config_key = "";
    result.config_value = "";

    return result;
}

ParsedCommand CommandParser::parseConfig(int argc, char** argv)
{
    if (argc == 2)
    {
        return makeError("config: missing key");
    }
    if (argc > 4)
    {
        return makeError("config: too many arguments");
    }

    std::string key = argv[2];
    if (key.find('.') == std::string::npos)
    {
        return makeError("config: invalid key format, expected <section>.<key>");
    }

    ParsedCommand result;
    result.command_type = CommandType::Config;
    result.valid = true;
    result.write = false;
    result.path = "";
    result.cat_file_mode = CatFileMode::None;
    result.error_msg = "";

    if (argc == 3)
    {
        result.config_key = key;
        result.config_value = "";
        return result;
    }
    else if (argc == 4)
    {
        result.config_key = key;
        result.config_value = argv[3];
        return result;
    }

    return makeError("config: unexpected error");
}


ParsedCommand CommandParser::parse(int argc, char **argv)
{
    if (argc < 2)
    {
        return makeError("No command provided");
    }

    std::string command = argv[1];

    if (command == "init")
    {
        return parseInit(argc, argv);
    }
    else if (command == "hash-object")
    {
        return parseHashObject(argc, argv);
    }
    else if (command == "cat-file")
    {
        return parseCatFile(argc, argv);
    }
    else if (command == "write-tree")
    {
        return parseWriteTree(argc, argv);
    }
    else if (command == "config")
    {
        return parseConfig(argc, argv);
    }
    else
    {
        return makeError("Unknown command: " + command);
    }
}