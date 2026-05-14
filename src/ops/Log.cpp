#include "Log.h"
#include "../objects/ParsedObject.h"
#include <iostream>
#include <sstream>

void printLog(const std::string& start_commit_id, ObjectStore& store)
{
    std::string current = start_commit_id;

    while (!current.empty())
    {
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

        std::cout << "commit " << current << std::endl;
        std::cout << "Author: " << author << " <" << email << ">" << std::endl;
        std::cout << "Date:   " << date << std::endl;
        std::cout << std::endl;
        while (std::getline(msg_stream, msg_line))
        {
            std::cout << "    " << msg_line << std::endl;
        }
        std::cout << std::endl;

        current = parent;
    }
}