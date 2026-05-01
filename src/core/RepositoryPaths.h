#pragma once
#include <filesystem>

class RepositoryPaths
{
    private:
        std::filesystem::path root_path;
    public:
        RepositoryPaths(std::filesystem::path root_path);
        
        std::filesystem::path workingRoot();
        std::filesystem::path gitDir();
        std::filesystem::path objectsDir();
        std::filesystem::path refsDir();
        std::filesystem::path headsDir();
        std::filesystem::path headFile();
        std::filesystem::path configFile();
        std::filesystem::path indexFile();
};