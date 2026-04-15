#include "cli/CommandParser.h"
#include "core/Repository.h"
#include <iostream>
#include <filesystem>

int main(int argc, char **argv)
{
    ParsedCommand parsed = CommandParser::parse(argc, argv);
    if (!parsed.valid)
    {
        std::cerr << parsed.error_msg << std::endl;
        return 1;
    }
    if (parsed.command_type == CommandType::Init)
    {
        if (parsed.path.empty())
        {
            std::filesystem::path path = std::filesystem::current_path();
            if (Repository::init(path))
            {
                std::cout << "Repository initialized successfully" << std::endl;
                return 0;
            }
            else
            {
                std::cerr << "Error" << std::endl;
                return 1;
            }
        }
        else
        {
            if (Repository::init(parsed.path))
            {
                std::cout << "Repository initialized successfully" << std::endl;
                return 0;
            }
            else
            {
                std::cerr << "Error" << std::endl;
                return 1;
            }
        }
    }

    return 0;
}