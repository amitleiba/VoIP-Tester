#pragma once

#include<iostream>
#include<string>
#include<sstream>
#include<iomanip>

#include"generic/Parser.hpp"
#include"VTCPOpcode.hpp"
#include"VTCPMessage.hpp"

class VTCPParser : public Parser
{
public:
    VTCPParser()
    {
        
    }

    ~VTCPParser() = default;

    std::string serialize(std::shared_ptr<Message> message) override
    {
        auto vtcpMessage = std::dynamic_pointer_cast<VTCPMessage>(message);
        std::stringstream ss;
        ss << std::setw(VTCPMessage::HEADER_LENGTH) << std::setfill('0') << vtcpMessage->getData().length();
        ss << std::setw(VTCPMessage::OPCODE_LENGTH) << std::setfill('0') << (int)vtcpMessage->getOpcode();
        std::string serializedMessage = ss.str() + vtcpMessage->getData();
        return serializedMessage;
    }

    std::shared_ptr<Message> deserialize(std::string serializedMessage) override
    {
        VTCPOpcode opcode = (VTCPOpcode)std::stoi(serializedMessage.substr(0, VTCPMessage::OPCODE_LENGTH));
        std::string data = serializedMessage.substr(VTCPMessage::OPCODE_LENGTH);
        return std::make_shared<VTCPMessage>(data, opcode);
    }

private:
   
};