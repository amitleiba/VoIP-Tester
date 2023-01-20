#pragma once

#include<boost/asio.hpp>

#include"VTCPParser.hpp"
#include"VTCPRequestHandler.hpp"
#include"tcp/TCPServer.hpp"
#include"generic/Parser.hpp"

class VTCPServer : public TCPServer
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
        return std::make_shared<VTCPParser>();
    }

    void onMessageReceived(const int id, std::shared_ptr<Message> message) override
    {
        auto vtcpMessage = std::dynamic_pointer_cast<VTCPMessage>(message);
        _handler.handle(vtcpMessage);
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