#pragma once
#include "GitActor.h"
#include "ObjectType.h"
#include <string>

class Commit
{
    private:
        std::string tree_id;
        std::string parent_id;
        GitActor author;
        GitActor commiter;
        std::string message;
    public:
        Commit(const std::string& tree_id, const std::string& parent_id, const GitActor& author, const GitActor& commiter, const std::string& message);
        const std::string& getTreeId() const;
        const std::string& getParentId() const;
        const GitActor& getAuthor() const;
        const GitActor& getCommiter() const;
        const std::string& getMessage() const;
        ObjectType type() const;
        std::string serialize() const;
};