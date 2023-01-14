#pragma once

#include <iostream>

#include<boost/asio.hpp>

#include"VTCPParser.hpp"
#include"VTCPRequestHandler.hpp"
#include"tcp/TCPServer.hpp"
#include"generic/Parser.hpp"

class VTCPServer : TCPServer
{
public:
    VTCPServer(const int port) :
        TCPServer(port)
    {
        
    }

    ~VTCPServer() = default;

private:

    std::shared_ptr<Parser> makeParser() override
    {
        return std::static_pointer_cast<Parser>(std::make_shared<VTCPParser>());
    }

    void onMessageReceived(const int id, Message message) override
    {
        _handler.handle(message);
        onCompletion(id);
    }

    void onCompletion(const int id) override
    {
        std::cout << "Done" << std::endl;
        //maybe send something to the client?
        //_sessions.at(id)->send(message)
    }

    VTCPRequestHandler _handler;
};