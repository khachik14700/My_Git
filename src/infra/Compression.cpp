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

std::string Compression::decompress(const std::string& data)
{
    if (data.empty())
    {
        return "";
    }

    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;

    int ret = inflateInit(&stream);
    if (ret != Z_OK)
    {
        throw std::runtime_error("Failed to initialize zlib inflate: " + std::to_string(ret));
    }

    stream.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(data.data()));
    stream.avail_in = static_cast<uInt>(data.size());

    std::string decompressed;
    decompressed.reserve(data.size() * 3);

    constexpr size_t CHUNK = 16384;
    char out[CHUNK];

    try
    {
        while (true)
        {
            stream.next_out = reinterpret_cast<Bytef*>(out);
            stream.avail_out = CHUNK;

            ret = inflate(&stream, Z_NO_FLUSH);

            const size_t have = CHUNK - stream.avail_out;
            if (have > 0)
            {
                decompressed.append(out, have);
            }

            if (ret == Z_STREAM_END)
            {
                break;
            }

            if (ret != Z_OK)
            {
                throw std::runtime_error("zlib inflate error: " + std::to_string(ret));
            }
        }
    }
    catch(...)
    {
        inflateEnd(&stream);
        throw;
    }

    inflateEnd(&stream);
    return decompressed;
}