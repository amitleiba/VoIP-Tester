#pragma once

#include<string>

#include"VTCPOpcode.hpp"
#include"generic/Message.hpp"

class VTCPMessage : public Message
{
public:
    VTCPMessage():
        Message()
    {

    }

    VTCPMessage(const std::string & data, const VTCPOpcode & opcode) :
        Message(), _data(data), _opcode(opcode)
    {

    }

    ~VTCPMessage() = default;

    std::string getData()
    {
        return _data;
    }

    VTCPOpcode getOpcode()
    {
        return _opcode;
    }

    void setData(const std::string & data)
    {
        _data = data;
    }

    void setOpcode(const VTCPOpcode & opcode)
    {
        _opcode = opcode;
    }

private:
    std::string _data;
    VTCPOpcode _opcode;
};