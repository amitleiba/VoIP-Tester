#pragma once

class Message
{
public:
    Message() = default;

    virtual ~Message() = default;
    
    static constexpr int HEADER_LENGTH = sizeof(int);
};