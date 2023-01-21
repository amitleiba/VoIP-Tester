#pragma once

class Message
{
public:
    Message() = default;

    virtual ~Message() = 0;

    static constexpr int OPCODE_LENGTH = sizeof(int);
    static constexpr int HEADER_LENGTH = sizeof(int);
};