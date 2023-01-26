#pragma once

#include"VTCPRequestHandler.hpp"
#include"tcp/TCPServer.hpp"

class VTCPServer : public TCPServer
{
public:
    VTCPServer(const int port) :
        TCPServer(port, std::make_shared<VTCPRequestHandler>())
    {
        
    }

    ~VTCPServer() = default;
};