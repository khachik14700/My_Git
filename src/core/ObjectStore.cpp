#include "ObjectStore.h"
#include "../infra/Hash.h"
#include "../infra/Compression.h"
#include "../infra/FileSystemUtils.h"

ObjectStore::ObjectStore(const std::filesystem::path& objects_dir)
{
    this->objects_dir = objects_dir;
}

std::string ObjectStore::writeObject(const std::string& raw_object)
{
    std::string object_id = Hash::sha256(raw_object);
    std::string dir_name = object_id.substr(0, 2);
    std::string file_name = object_id.substr(2);

    std::filesystem::path dir_path = this->objects_dir / dir_name;
    std::filesystem::path file_path = dir_path / file_name;

    if (FileSystemUtils::exists(file_path) && FileSystemUtils::isRegularFile(file_path))
    {
        return object_id;
    }

    if (!FileSystemUtils::createDirectories(dir_path))
    {
        return "";
    }

    try
    {
        std::string compressed_object = Compression::compress(raw_object); //error handling

        if (!FileSystemUtils::writeBinaryFile(file_path, compressed_object))
        {
            return "";
        }

        return object_id;
    }
    catch (const std::runtime_error&)
    {
        return "";
    }
}

std::string ObjectStore::readObject(const std::string& object_id) const
{
    if (object_id.size() != 64)
    {
        return "";
    }

    std::string dir_name = object_id.substr(0, 2);
    std::string file_name = object_id.substr(2);

    std::filesystem::path dir_path = objects_dir / dir_name;
    std::filesystem::path file_path = dir_path / file_name;

    if (!FileSystemUtils::exists(file_path) || !FileSystemUtils::isRegularFile(file_path))
    {
        return "";
    }

    std::string compressed_object;
    if (!FileSystemUtils::readBinaryFile(file_path, compressed_object))
    {
        return "";
    }
    try
    {
        std::string raw_object = Compression::decompress(compressed_object);
        return raw_object;
    }
    catch(const std::runtime_error&)
    {
        return "";
    }
}