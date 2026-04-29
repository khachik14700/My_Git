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

std::string Hash::hexToBytes(const std::string& hex)
{
    if (hex.size() % 2 != 0)
    {
        throw std::runtime_error("Invalid hex string length");
    }
    std::string result = "";
    for (size_t i = 0; i < hex.size(); i += 2)
    {
        std::string two_chars = hex.substr(i, 2);
        unsigned char byte = static_cast<unsigned char>(std::stoul(two_chars, nullptr, 16));
        result += static_cast<char>(byte);
    }
    return result;
}

std::string Hash::bytesToHex(const std::string& bytes)
{
    std::string hex = "0123456789abcdef";
    std::string result = "";
    for (char byte : bytes)
    {
        int number = static_cast<unsigned char>(byte);
        unsigned char first = hex[number / 16];
        unsigned char second = hex[number % 16];
        result += first;
        result += second;
    }
    return result;
}