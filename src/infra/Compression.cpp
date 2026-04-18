#include "Compression.h"
#include <zlib.h>
#include <stdexcept>

std::string Compression::compress(const std::string& data)
{
    if (data.empty())
    {
        return "";
    }

    uLongf destLen = compressBound(static_cast<uLong>(data.size()));
    std::string result(destLen, '\0');

    int ret = ::compress(
        reinterpret_cast<Bytef*>(result.data()),
        &destLen,
        reinterpret_cast<const Bytef*>(data.data()),
        static_cast<uLong>(data.size())
    );

    if (ret != Z_OK)
    {
        throw std::runtime_error("Compression failed with error code: " + std::to_string(ret));
    }

    result.resize(destLen);
    return result;
}