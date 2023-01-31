#pragma once

#include"VTCPRequestHandler.hpp"
#include"tcp/TCPServer.hpp"

class VTCPServer : public TCPServer
{
public:
    VTCPServer(const std::uint16_t port) :
        TCPServer(port, std::make_shared<VTCPRequestHandler>())
    {
    }

    ~VTCPServer() = default;
};