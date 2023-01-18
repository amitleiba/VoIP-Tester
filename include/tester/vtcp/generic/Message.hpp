#pragma once

#include<iostream>
#include<../VTCPOpcodes.hpp>

struct Message
{
    VTCPOpcodes opcode;
    std::string data;
};
