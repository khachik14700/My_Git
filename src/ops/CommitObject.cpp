#include "CommitObject.h"
#include <ctime>
#include <sstream>
#include <iomanip>

static std::string getTimezone()
{
    std::time_t t= std::time(nullptr);
    std::tm local_tm = {};

    #ifdef _WIN32
        localtime_s(&local_tm, &t);
    #else
        localtime_r(&t, &local_tm);
    #endif

    std::ostringstream oss;

    oss << std::put_time(&local_tm, "%z");
    return oss.str();
}

std::string createCommit(const std::string& tree_id, const std::string& parent_id, const std::string& message, const GitActor& author, const GitActor& commiter, ObjectStore& store)
{
    std::string timezone = getTimezone();
    GitActor actual_author = author;
    actual_author.timestamp = static_cast<std::int64_t>(std::time(nullptr));
    actual_author.timezone = timezone;

    GitActor actual_commiter = commiter;
    actual_commiter.timestamp = static_cast<std::int64_t>(std::time(nullptr));
    actual_commiter.timezone = timezone;

    Commit commit(tree_id, parent_id, actual_author, actual_commiter, message);
    std::string commit_id = store.writeObject(commit.serialize());
    return commit_id;
}