#pragma once

enum class CommandType
{
    Init,
    HashObject,
    CatFile,
    WriteTree,
    Config,
    Add,
    Rm,
    Commit,
    Branch,
    Switch,
    Status,
    Log,
    Unknown
};