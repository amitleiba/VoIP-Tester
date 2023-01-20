#pragma once

class Message
{
public:
    Message() = default;

    ~Message() = default;

    static constexpr int OPCODE_LENGTH = sizeof(int);
    static constexpr int HEADER_LENGTH = sizeof(int);
protected:

};