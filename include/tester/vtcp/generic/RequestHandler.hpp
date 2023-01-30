#pragma once

#include<memory>

#include"Message.hpp"
#include"../tcp/TCPSession.hpp"

class RequestHandler
{
public:
    RequestHandler() = default;

    virtual ~RequestHandler() = default;

    virtual void handle(std::shared_ptr<TCPSession> sender, const Message& request) = 0;
};