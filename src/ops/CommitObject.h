#include "../objects/Commit.h"
#include "../objects/GitActor.h"
#include "../core/ObjectStore.h"

std::string createCommit(const std::string& tree_id, const std::string& parent_id, const std::string& message, const GitActor& author, const GitActor& commiter, ObjectStore& store);