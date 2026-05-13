#include "Log.h"
#include "../objects/ParsedObject.h"
#include <iostream>

void printLog(const std::string& start_commit_id, ObjectStore& store)
{
    std::string current = start_commit_id;

    while (!current.empty())
    {
        std::string raw = store.readObject(current);
        if (raw.empty())
            break;
        ParsedObject obj = ParsedObject::parse(raw);

        std::string parent = obj.parseCommitParentId();
        std::string author = obj.parseCommitAuthor();
        std::string message = obj.parseCommitMessage();

        std::cout << "commit " << current << std::endl;
        std::cout << "Author: " << author << std::endl;
        std::cout << std::endl;
        std::cout << "\t" << message << std::endl;
        std::cout << std::endl;

        current = parent;
    }
}