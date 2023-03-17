#pragma once

#include <functional>

#include"VTCPSession.hpp"
#include"tcp/TCPServer.hpp"

class VTCPServer : public TCPServer<VTCPSession>
{
public:
    VTCPServer(const std::uint16_t port, 
    std::function<bsoncxx::document::value(int)> startAutoTest,
    std::function<void()> startManualTest) :
        TCPServer(port, startAutoTest, startManualTest)
    {
    }

    ~VTCPServer() = default;
};