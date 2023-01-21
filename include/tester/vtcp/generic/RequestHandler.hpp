#pragma once

#include<memory>

#include"Message.hpp"

class RequestHandler
{
public:
    RequestHandler()
    {

    }

    ~RequestHandler() = default;

    virtual void handle(std::shared_ptr<Message> message) = 0;

protected:

private:

};
