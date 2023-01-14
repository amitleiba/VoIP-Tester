#pragma once

// class Message
// {
// public:
//     Message(/* args */)
//     {

//     }

//     ~Message() = default;

// private:
//     int _opCode;
//     std::string _data;

// };

struct Message
{
    int opcode;
    std::string data;
};
