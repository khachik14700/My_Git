#include "Blob.h"
#include "ObjectHeader.h"

Blob::Blob(const std::string& data)
{
    this->data = data;
}

ObjectType Blob::type() const
{
    return ObjectType::Blob;
}

std::string Blob::payload() const
{
    return data;
}

std::string Blob::serialize() const
{
    std::string payload = this->payload();
    std::string header = buildObjectHeader(this->type(), payload.size());
    return header + payload;
}