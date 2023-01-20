#pragma once

#include<../VTCPOpcode.hpp>

struct Message
{
    VTCPOpcode opcode;
    std::string data;
};
