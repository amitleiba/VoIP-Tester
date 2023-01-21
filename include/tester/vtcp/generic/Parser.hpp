#pragma once

#include<string>
#include<memory>

#include"Message.hpp"

class Parser
{
public:
    Parser()
    {

    }

    virtual ~Parser() = 0;

    virtual std::string serialize(std::shared_ptr<Message> message) =  0;
    virtual std::shared_ptr<Message> deserialize(std::string data) = 0;
};