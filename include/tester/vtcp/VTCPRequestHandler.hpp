#pragma once

#include<iostream>
#include<functional>
#include<memory>
#include<unordered_map>

#include"generic/Message.hpp"

class VTCPRequestHandler
{
public:
    VTCPRequestHandler()
    {

    }

    void handle(Message message)
    {
        try
        {
            _handlers.at(message.opcode)(message.data);
        }
        catch(const std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }
    }

    ~VTCPRequestHandler() = default;

private:
    std::unordered_map<const int, std::function<void(std::string)>> _handlers;
};