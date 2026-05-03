#include "RepositoryPaths.h"

RepositoryPaths::RepositoryPaths(std::filesystem::path root_path)
{
    this->root_path = root_path;
}

std::filesystem::path RepositoryPaths::workingRoot()
{
    return root_path;
}

std::filesystem::path RepositoryPaths::gitDir()
{
    return workingRoot()/".git";
}

std::filesystem::path RepositoryPaths::objectsDir()
{
    return gitDir()/"objects";
}

std::filesystem::path RepositoryPaths::refsDir()
{
    return gitDir()/"refs";
}

std::filesystem::path RepositoryPaths::headsDir()
{
    return refsDir()/"heads";
}

std::filesystem::path RepositoryPaths::headFile()
{
    return gitDir()/"HEAD";
}

std::filesystem::path RepositoryPaths::configFile()
{
    return gitDir()/"config";
}

std::filesystem::path RepositoryPaths::indexFile()
{
    return gitDir()/"index";
}

std::filesystem::path RepositoryPaths::commitEditMsgFile()
{
    return gitDir() / "COMMIT_EDITMSG";
}