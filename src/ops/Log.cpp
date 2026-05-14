#include "Log.h"
#include "../objects/ParsedObject.h"
#include <iostream>
#include <sstream>

void printLog(const std::string& start_commit_id, ObjectStore& store, bool oneline, int max_count)
{
    std::string current = start_commit_id;
    int count = 0;

    while (!current.empty())
    {
        if (max_count > 0 && count >= max_count)
            break;

        std::string raw = store.readObject(current);
        if (raw.empty())
        {
            throw std::runtime_error("Error: Could not read commit " + current);
        }
        ParsedObject obj = ParsedObject::parse(raw);

        std::string parent = obj.parseCommitParentId();
        std::string author = obj.parseCommitAuthorName();
        std::string email = obj.parseCommitAuthorEmail();
        std::string date = obj.parseCommitTimestamp();
        std::string message = obj.parseCommitMessage();
        std::istringstream msg_stream(message);
        std::string msg_line;

        if (oneline)
        {
            std::getline(msg_stream, msg_line);
            std::cout << current.substr(0,8) << " " << msg_line << std::endl;
        }
        else
        {
            std::cout << "commit " << current << std::endl;
            std::cout << "Author: " << author << " <" << email << ">" << std::endl;
            std::cout << "Date:   " << date << std::endl;
            std::cout << std::endl;
            while (std::getline(msg_stream, msg_line))
            {
                std::cout << "    " << msg_line << std::endl;
            }
            std::cout << std::endl;
        }

        count++;
        current = parent;
    }
}