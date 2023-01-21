#pragma once

#include"VTCPParser.hpp"
#include"VTCPRequestHandler.hpp"
#include"tcp/TCPServer.hpp"

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

    std::shared_ptr<RequestHandler> makeRequestHandler() override
    {
        return std::make_shared<VTCPRequestHandler>();
    }
    
};