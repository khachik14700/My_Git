#include "Repository.h"
#include "RepositoryPaths.h"
#include "RepositoryConfig.h"
#include "../infra/FileSystemUtils.h"


bool Repository::exists(const std::filesystem::path& path)
{
    RepositoryPaths paths(path);
    std::filesystem::path git_path = paths.gitDir();
    if (FileSystemUtils::exists(git_path))
    {
        if (FileSystemUtils::isDirectory(git_path))
        {
            return true;
        }
    }
    return false;
}

bool Repository::isValid(const std::filesystem::path& path)
{
    RepositoryPaths paths(path);
    if (!Repository::exists(path))
    {
        return false;
    }
    if (!FileSystemUtils::exists(paths.objectsDir()) || !FileSystemUtils::isDirectory(paths.objectsDir()))
    {
        return false;
    }
    if (!FileSystemUtils::exists(paths.refsDir()) || !FileSystemUtils::isDirectory(paths.refsDir()))
    {
        return false;
    }
    if (!FileSystemUtils::exists(paths.headsDir()) || !FileSystemUtils::isDirectory(paths.headsDir()))
    {
        return false;
    }
    if (!FileSystemUtils::exists(paths.headFile()) || !FileSystemUtils::isRegularFile(paths.headFile()))
    {
        return false;
    }
    if (!FileSystemUtils::exists(paths.configFile()) || !FileSystemUtils::isRegularFile(paths.configFile()))
    {
        return false;
    }
    return true;
}

bool Repository::init(const std::filesystem::path& path)
{
    if (FileSystemUtils::exists(path))
    {
        if (!FileSystemUtils::isDirectory(path))
        {
            return false;
        }
    }
    if (FileSystemUtils::createDirectories(path))
    {
        if (Repository::exists(path))
        {
            if (Repository::isValid(path))
            {
                return true;
            }
            return false;
        }
        RepositoryPaths paths(path);
        if (!FileSystemUtils::createDirectories(paths.gitDir()) || !FileSystemUtils::createDirectories(paths.objectsDir())
            || !FileSystemUtils::createDirectories(paths.refsDir()) || !FileSystemUtils::createDirectories(paths.headsDir()))
        {
            return false;
        }
        RepositoryConfig config;
        if (!FileSystemUtils::writeTextFile(paths.headFile(), "ref: refs/heads/main\n"))
        {
            return false;
        }
        if (!FileSystemUtils::writeTextFile(paths.configFile(), config.serialize()))
        {
            return false;
        }
    }
    else
    {
        return false;
    }
    return Repository::isValid(path);
}