#pragma once

#include<string>

#include"Message.hpp"

class Parser
{
public:
    Parser()
    {

    }

    ~Parser() = default;

    virtual std::string serialize(Message message) =  0;
    virtual Message deserialize(std::string data) = 0;

protected:
    static constexpr int OPCODE_LENGTH = sizeof(int);
    static constexpr int HEADER_LENGTH = sizeof(int);

private:

};