#pragma once

class Message
{
public:
    Message() = default;

    virtual ~Message();

    static constexpr int HEADER_LENGTH = sizeof(int);
};