#include "cli/CommandParser.h"
#include "core/Repository.h"
#include "objects/Blob.h"
#include "infra/Hash.h"
#include <iostream>
#include <filesystem>

int main(int argc, char **argv)
{
    Blob blob("hello");
    std::string raw = blob.serialize();
    std::string id = Hash::sha256(raw);
    std::cout << blob.payload() << std::endl;
    std::cout << raw.size() << " raw : " << raw << std::endl;
    std::cout << id << std::endl;


    /*-----------------------------------------------------*/
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