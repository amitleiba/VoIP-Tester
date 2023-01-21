#pragma once

#include<memory>

#include"Message.hpp"

class RequestHandler
{
public:
    RequestHandler() = default;

    virtual ~RequestHandler() = 0;

    virtual void handle(std::shared_ptr<Message> message) = 0;
};
