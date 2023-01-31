#pragma once

#include<vector>
#include<string>
#include<cstring>

class Message
{
public:
    Message():
        _index(0)
    {
    }

    Message(const std::vector<std::uint8_t> & payload):
        _payload(payload), _index(0)
    {
    }

    ~Message() = default;
        
    //TODO: add readInteger, readString
    int readInteger() const
    {
        if(!canRead(sizeof(int)))
            throw;
        int value;
        std::memcpy(&value, &_payload[_index], sizeof(int));
        _index += sizeof(int);
        return value;
    }

    std::string readString() const
    {
        auto size = readInteger();
        if(!canRead(size))
        {
            throw;
        }
        return readString(size);
    }

    std::uint8_t readByte() const
    {
        if(!canRead(sizeof(std::uint8_t)))
            throw;
        std::uint8_t value = _payload.at(_index++);
        return value;
    }

    bool canRead(std::size_t size) const
    {
        return size > 0 && _index + size <= _payload.size();
    }

    void push(int integer)
    {
        std::uint8_t *data = reinterpret_cast<std::uint8_t*>(&integer);
        _payload.insert(_payload.end(), data, data + sizeof(int));
    }

    void push(const std::string & string)
    {
        push(string.size());
        _payload.insert(_payload.end(), string.begin(), string.end());
    }

    void pushSize()
    {
        int size = _payload.size();
        std::vector<uint8_t> size_bytes(sizeof(size));
        std::memcpy(size_bytes.data(), &size, sizeof(size));
        _payload.insert(_payload.begin(), size_bytes.begin(), size_bytes.end());
    }

    std::vector<std::uint8_t> getPayload() const
    {
        return _payload;
    }

    std::size_t getIndex() const
    {
        return _index;
    }


private:

    std::string readString(std::size_t size) const
    {
        auto iterator = _payload.begin() + _index;
        std::string value(iterator, iterator + size); 
        _index += size;
        return value;
    }

    std::vector<std::uint8_t> _payload;
    mutable std::size_t _index;
};