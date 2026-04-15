#pragma once
#include <string>

class RepositoryConfig
{
    private:
        int repository_format_version;
        bool filemode;
        bool bare;
    public:
        RepositoryConfig();
        std::string serialize();
};