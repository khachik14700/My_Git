#include "RepositoryConfig.h"

RepositoryConfig::RepositoryConfig()
{
    this->repository_format_version = 0;
    this->filemode = true;
    this->bare = false;
}

std::string RepositoryConfig::serialize()
{
    std::string result = "[core]\n";
    result += "\trepositoryformatversion = " + std::to_string(repository_format_version) + "\n";
    result += "\tfilemode = " + std::string(filemode ? "true" : "false") + "\n";
    result += "\tbare = " + std::string(bare ? "true" : "false") + "\n";
    return result;
}