#include "Editor.h"
#include "../infra/FileSystemUtils.h"
#include <cstdlib>
#include <sstream>
#include <stdexcept>

std::string openEditor(const std::filesystem::path& file_path)
{
    std::string write_content = "\n# Please enter the commit message above.\n# Lines starting with '#' will be ignored.\n";
    if (!FileSystemUtils::writeTextFile(file_path, write_content))
    {
        throw std::runtime_error("Error: Failed to create temporary file for editor");
    }

    const char* editor = std::getenv("EDITOR");
    std::string editor_cmd = editor ? editor : "nano";

    std::string cmd = editor_cmd + ' ' + file_path.string();
    int result = std::system(cmd.c_str());

    if (result != 0)
    {
        throw std::runtime_error("Error: editor failed to run");
    }

    std::string read_content = "";
    if (!FileSystemUtils::readBinaryFile(file_path, read_content))
    {
        throw std::runtime_error("Failed to read file after editing");
    }

    if (read_content.empty())
    {
        throw std::runtime_error("Error: commit message file is empty");
    }

    std::stringstream ss(read_content);
    std::string line;
    std::string message;

    while (std::getline(ss, line))
    {
        if (!line.empty() && line[0] == '#')
        {
            continue;
        }

        message += line;
        message += '\n';
    }
    if (message.empty())
    {
        throw std::runtime_error("Error: empty commit message");
    }

    size_t start = message.find_first_not_of("\n\r");
    if (start == std::string::npos)
    {
        throw std::runtime_error("Error: empty commit message");
    }

    message = message.substr(start);

    size_t end = message.find_last_not_of("\n\r");
    if (end != std::string::npos)
    {
        message = message.substr(0, end + 1) + "\n";
    }

    return message;
}