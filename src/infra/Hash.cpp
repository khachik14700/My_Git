#include "Hash.h"
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

std::string Hash::sha256(const std::string& data)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];

    SHA256(
        reinterpret_cast<const unsigned char*>(data.data()),
        data.length(),
        hash
    );

    std::ostringstream oss;
    oss << std::hex << std::setfill('0');

    for (unsigned char byte : hash)
    {
        oss << std::setw(2) << static_cast<unsigned int>(byte);
    }

    return oss.str();
}