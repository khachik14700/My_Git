#include "Commit.h"
#include "ObjectHeader.h"

Commit::Commit(const std::string& tree_id, const std::string& parent_id, const GitActor& author, const GitActor& commiter, const std::string& message)
    : tree_id(tree_id), parent_id(parent_id), author(author), commiter(commiter), message(message) {}

ObjectType Commit::type() const
{
    return ObjectType::Commit;
}

const std::string& Commit::getTreeId() const
{
    return this->tree_id;
}

const std::string& Commit::getParentId() const
{
    return this->parent_id;
}

const GitActor& Commit::getAuthor() const
{
    return this->author;
}

const GitActor& Commit::getCommiter() const
{
    return this->commiter;
}

const std::string& Commit::getMessage() const
{
    return this->message;
}

std::string Commit::serialize() const
{
    std::string actor_string = author.name + " " + "<" + author.email + ">" + " " + std::to_string(author.timestamp) + " " + author.timezone;
    std::string committer_string = commiter.name + " " + "<" + commiter.email + ">" + " " + std::to_string(commiter.timestamp) + " " + commiter.timezone;
    std::string payload = "";
    payload += "tree " + tree_id + "\n";
    if (!parent_id.empty())
    {
        payload += "parent " + parent_id + "\n";
    }
    payload += "author " + actor_string + "\n";
    payload += "committer " + committer_string + "\n";
    payload += "\n";
    payload += message + "\n";
    std::string header = buildObjectHeader(type(), payload.size());
    return header + payload;
}