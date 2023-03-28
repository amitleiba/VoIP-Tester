#pragma once

#include <functional>

#include"VTCPSession.hpp"
#include"tcp/TCPServer.hpp"

class VTCPServer : public TCPServer<VTCPSession>
{
public:
    VTCPServer(const std::uint16_t port, 
    std::function<bsoncxx::document::value(int)> startAutoTest,
    std::function<void()> startManualTest,
    std::function<bsoncxx::document::value()> getHistoryHeaders,
    std::function<bsoncxx::document::value(const std::string &)> getHistoryLog) :
        TCPServer(port, startAutoTest, startManualTest, getHistoryHeaders, getHistoryLog)
    {
    }

    ~VTCPServer() = default;
};