#pragma once

#include<iostream>
#include<string>
#include<sstream>
#include<iomanip>

#include"generic/Parser.hpp"
#include"VTCPOpcode.hpp"

class VTCPParser : public Parser
{
public:
    VTCPParser()
    {
        
    }

    ~VTCPParser() = default;

    std::string serialize(Message message) override
    {
        std::stringstream ss;
        ss << std::setw(HEADER_LENGTH) << std::setfill('0') << message.data.length();
        ss << std::setw(OPCODE_LENGTH) << std::setfill('0') << (int)message.opcode;
        std::string serializedMessage = ss.str() + message.data;
        return serializedMessage;
    }

    Message deserialize(std::string serializedMessage) override
    {
        VTCPOpcode opcode = (VTCPOpcode)std::stoi(serializedMessage.substr(0, OPCODE_LENGTH));
        std::string data = serializedMessage.substr(OPCODE_LENGTH);
        return Message{opcode, data};
    }

private:
   
};