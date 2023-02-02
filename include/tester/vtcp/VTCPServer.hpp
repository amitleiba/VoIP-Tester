#pragma once

#include"VTCPSession.hpp"
#include"tcp/TCPServer.hpp"

class VTCPServer : public TCPServer<VTCPSession>
{
public:
    VTCPServer(const std::uint16_t port) :
        TCPServer(port)
    {
    }

    ~VTCPServer() = default;
};