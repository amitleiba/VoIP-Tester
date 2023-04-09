#pragma once

#include <functional>

#include "tcp/TCPServer.hpp"
#include "VTCPOpcode.hpp"

class VTCPServer : public TCPServer
{
public:
    VTCPServer(const std::uint16_t port, 
    std::function<bsoncxx::document::value(int)> startAutoTest,
    std::function<void()> startManualTest,
    std::function<bsoncxx::document::value()> getHistoryHeaders,
    std::function<bsoncxx::document::value(const std::string &)> getHistoryLog) :
        TCPServer(port),
        _startAutoTest(std::move(startAutoTest)),
        _startManualTest(std::move(startManualTest)),
        _getHistoryHeaders(std::move(getHistoryHeaders)),
        _getHistoryLog(std::move(getHistoryLog))
    {
        _handlers.emplace(VTCPOpcode::VTCP_CONNECT_REQ, std::bind(&VTCPServer::onVtcpConnect, this, std::placeholders::_1, std::placeholders::_2));
        _handlers.emplace(VTCPOpcode::VTCP_DISCONNECT_REQ, std::bind(&VTCPServer::onVtcpDisconnect, this, std::placeholders::_1, std::placeholders::_2));
        _handlers.emplace(VTCPOpcode::VTCP_AUTO_TEST_REQ, std::bind(&VTCPServer::onVtcpAutoTest, this, std::placeholders::_1, std::placeholders::_2));
        _handlers.emplace(VTCPOpcode::VTCP_MANUAL_TEST_REQ, std::bind(&VTCPServer::onVtcpManualTest, this, std::placeholders::_1, std::placeholders::_2));
        _handlers.emplace(VTCPOpcode::VTCP_HISTORY_HEADER_REQ, std::bind(&VTCPServer::onVtcpHistoryHeader, this, std::placeholders::_1, std::placeholders::_2));
        _handlers.emplace(VTCPOpcode::VTCP_HISTORY_LOG_REQ, std::bind(&VTCPServer::onVtcpHistoryLog, this, std::placeholders::_1, std::placeholders::_2));
    }

    ~VTCPServer() = default;

    void handle(const std::size_t id, const Message& request) override
    {
        try
        {
            auto op = request.readInteger();
            std::cout << op << std::endl;
            auto opcode = static_cast<VTCPOpcode>(op);
            _handlers.at(opcode)(id, request);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    void onVtcpConnect(const std::size_t id, const Message & request)
    {
        std::cout << "New client connected" << std::endl;

        Message response;
        response.push(static_cast<int>(VTCPOpcode::VTCP_CONNECT_RES));
        send(id, response);
    }

    void onVtcpDisconnect(const std::size_t id, const Message & request)
    {
        std::cout << "Client disconnected" << std::endl;

        try
        {
            _sessions.at(id)->disconnect();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    void onVtcpAutoTest(const std::size_t id, const Message & request)
    {
        std::cout << "Client requested Auto test" << std::endl;

        std::string domain = request.readString();
        int amount = request.readInteger();

        std::cout << "Domain: " << domain << " Amount: " << amount << std::endl;

        Message response;

        response.push(static_cast<int>(VTCPOpcode::VTCP_AUTO_TEST_RES));
        response.push(bsoncxx::to_json(_startAutoTest(amount).view()));
        send(id, response);
    }

    void onVtcpManualTest(const std::size_t id, const Message & request)
    {
        std::cout << "Client requested Manual test" << std::endl;

        _startManualTest();
    }

    void onVtcpHistoryHeader(const std::size_t id, const Message & request)
    {
        Message response;

        response.push(static_cast<int>(VTCPOpcode::VTCP_HISTORY_HEADER_RES));
        response.push(bsoncxx::to_json(_getHistoryHeaders()));
        send(id, response);
    }

    void onVtcpHistoryLog(const std::size_t id, const Message & request)
    {
        auto doc_id = request.readString();
        Message response;

        response.push(static_cast<int>(VTCPOpcode::VTCP_HISTORY_LOG_RES));
        response.push(bsoncxx::to_json(_getHistoryLog(doc_id)));
        send(id, response);
    }

private:
    std::unordered_map<VTCPOpcode, std::function<void(int ,const Message &)>> _handlers;
    std::function<bsoncxx::document::value(int)> _startAutoTest;
    std::function<void()> _startManualTest;
    std::function<bsoncxx::document::value()> _getHistoryHeaders;
    std::function<bsoncxx::document::value(const std::string &)> _getHistoryLog;
};